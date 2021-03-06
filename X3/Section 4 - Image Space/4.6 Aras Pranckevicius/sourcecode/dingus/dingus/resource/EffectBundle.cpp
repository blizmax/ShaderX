// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// --------------------------------------------------------------------------
#include "stdafx.h"

#include "EffectBundle.h"
#include "../utils/Errors.h"
#include "../kernel/D3DDevice.h"

using namespace dingus;


CEffectBundle::CEffectBundle()
:	mSharedPool(0), 
	mOptimizeShaders(true), mUseStateManager(true), mLastErrors("")
{
	addExtension( ".fx" );
	mMacros.reserve( 16 );
	// last macro must be NULL
	mMacros.push_back( D3DXMACRO() );
	mMacros.back().Name = mMacros.back().Definition = NULL;
};

void CEffectBundle::setMacro( const char* name, const char* value )
{
	int idx = findMacro( name );
	if( idx >= 0 ) {
		// have existing one, replace
		mMacros[idx].Definition = value;
	} else {
		// replace last (which was NULL)
		int lastIdx = mMacros.size()-1;
		mMacros[lastIdx].Name = name;
		mMacros[lastIdx].Definition = value;
		// last macro must be NULL
		mMacros.push_back( D3DXMACRO() );
		mMacros.back().Name = mMacros.back().Definition = NULL;
	}
}

void CEffectBundle::removeMacro( const char* name )
{
	int idx = findMacro( name );
	if( idx >= 0 ) {
		// copy pre-last in place of removed one (last one is NULL)
		int preLastIdx = mMacros.size()-2;
		mMacros[idx] = mMacros[preLastIdx];
		// set pre-last to NULL
		mMacros[preLastIdx].Name = mMacros[preLastIdx].Definition = NULL;
		// remove last
		mMacros.pop_back();
	}
}

int CEffectBundle::findMacro( const char* name ) const
{
	int n = mMacros.size() - 1; // last one is NULL anyway
	for( int i = 0; i < n; ++i ) {
		if( !strcmp(name,mMacros[i].Name) )
			return i;
	}
	return -1; // not found
}


ID3DXEffect* CEffectBundle::loadEffect( const CResourceId& id, const CResourceId& fullName ) const
{
	ID3DXEffect* fx = NULL;
	ID3DXBuffer* errors = NULL;

	mLastErrors = "";

	assert( mSharedPool );
	HRESULT hres = D3DXCreateEffectFromFile(
		&CD3DDevice::getInstance().getDevice(),
		fullName.getUniqueName().c_str(),
		&mMacros[0],
		NULL, // TBD ==> includes
		mOptimizeShaders ? 0 : D3DXSHADER_SKIPOPTIMIZATION,
		mSharedPool,
		&fx,
		&errors );
	if( errors && errors->GetBufferSize() > 1 ) {
		std::string msg = "messages compiling effect '" + fullName.getUniqueName() + "'";
		mLastErrors = (const char*)errors->GetBufferPointer();
		msg += mLastErrors;
		CConsole::CON_ERROR.write( msg );
	}

	if( FAILED( hres ) ) {
		return NULL;
	}
	assert( fx );

	CONSOLE.write( "fx loaded '" + id.getUniqueName() + "'" );

	if( errors )
		errors->Release();

	// set state manager
	if( mUseStateManager )
		fx->SetStateManager( &CD3DDevice::getInstance().getStateManager() );

	return fx;
}

CD3DXEffect* CEffectBundle::loadResourceById( const CResourceId& id, const CResourceId& fullName )
{
	ID3DXEffect* fx = loadEffect( id, fullName );
	if( !fx )
		return NULL;
	return new CD3DXEffect( fx );
}

void CEffectBundle::createResource()
{
	// create pool
	assert( !mSharedPool );
	D3DXCreateEffectPool( &mSharedPool );
	assert( mSharedPool );

	// reload all objects
	TResourceMap::iterator it;
	for( it = mResourceMap.begin(); it != mResourceMap.end(); ++it ) {
		CD3DXEffect& res = *it->second;
		assert( res.isNull() );
		CD3DXEffect* n = tryLoadResourceById( it->first );
		assert( n );
		res.setObject( n->getObject() );
		delete n;
		assert( !res.isNull() );
	}
}

void CEffectBundle::activateResource()
{
	// call reset on effects
	TResourceMap::iterator it;
	for( it = mResourceMap.begin(); it != mResourceMap.end(); ++it ) {
		CD3DXEffect& res = *it->second;
		assert( !res.isNull() );
		res.getObject()->OnResetDevice();
	}
}

void CEffectBundle::passivateResource()
{
	// call lost on effects
	TResourceMap::iterator it;
	for( it = mResourceMap.begin(); it != mResourceMap.end(); ++it ) {
		CD3DXEffect& res = *it->second;
		assert( !res.isNull() );
		res.getObject()->OnLostDevice();
	}
}

void CEffectBundle::deleteResource()
{
	// unload all objects
	TResourceMap::iterator it;
	for( it = mResourceMap.begin(); it != mResourceMap.end(); ++it ) {
		CD3DXEffect& res = *it->second;
		assert( !res.isNull() );
		res.getObject()->Release();
		res.setObject( NULL );
	}

	// release pool
	assert( mSharedPool );
	mSharedPool->Release();
	mSharedPool = NULL;
}


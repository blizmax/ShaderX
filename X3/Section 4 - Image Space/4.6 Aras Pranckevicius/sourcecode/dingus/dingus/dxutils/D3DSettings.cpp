// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// --------------------------------------------------------------------------
#include "stdafx.h"
#include <windowsx.h>

#include "D3DSettings.h"
#include "D3DApplication.h"
#include "resource.h"

#include "DXUtil.h"
#include "D3DUtil.h"

using namespace dingus;


static CD3DSettingsDialog*	gSettingsDialog = NULL;
static CD3DApplication*		gSettingsApp = NULL;


//---------------------------------------------------------------------------

/**
 *  Returns the string for the given D3DDEVTYPE.
 */
TCHAR* gD3DDevTypeToString( D3DDEVTYPE devType )
{
	switch( devType ) {
	case D3DDEVTYPE_HAL:	return TEXT("HW accelerated");
	case D3DDEVTYPE_SW: 	return TEXT("Software");
	case D3DDEVTYPE_REF:	return TEXT("Reference");
	default:				return TEXT("Unknown");
	}
};

/**
 *  Returns the string for the given D3DMULTISAMPLE_TYPE.
 */
TCHAR* gMultiSampleTypeToString( D3DMULTISAMPLE_TYPE msType )
{
	switch( msType ) {
	case D3DMULTISAMPLE_NONE:			return TEXT("None");
	case D3DMULTISAMPLE_NONMASKABLE:	return TEXT("Nonmaskable");
	case D3DMULTISAMPLE_2_SAMPLES:		return TEXT("2x");
	case D3DMULTISAMPLE_3_SAMPLES:		return TEXT("3x");
	case D3DMULTISAMPLE_4_SAMPLES:		return TEXT("4x");
	case D3DMULTISAMPLE_5_SAMPLES:		return TEXT("5x");
	case D3DMULTISAMPLE_6_SAMPLES:		return TEXT("6x");
	case D3DMULTISAMPLE_7_SAMPLES:		return TEXT("7x");
	case D3DMULTISAMPLE_8_SAMPLES:		return TEXT("8x");
	case D3DMULTISAMPLE_9_SAMPLES:		return TEXT("9x");
	case D3DMULTISAMPLE_10_SAMPLES:		return TEXT("10x");
	case D3DMULTISAMPLE_11_SAMPLES:		return TEXT("11x");
	case D3DMULTISAMPLE_12_SAMPLES:		return TEXT("12x");
	case D3DMULTISAMPLE_13_SAMPLES:		return TEXT("13x");
	case D3DMULTISAMPLE_14_SAMPLES:		return TEXT("14x");
	case D3DMULTISAMPLE_15_SAMPLES:		return TEXT("15x");
	case D3DMULTISAMPLE_16_SAMPLES:		return TEXT("16x");
	default:							return TEXT("Unknown");
	}
};

/**
 *  Returns the string for the given vertex processing.
 */
TCHAR* gVertexProcessingToString( eVertexProcessing vpt )
{
	switch( vpt ) {
	case SOFTWARE_VP:	   return TEXT("Software");
	case MIXED_VP:		   return TEXT("Mixed");
	case HARDWARE_VP:	   return TEXT("Hardware");
	case PURE_HARDWARE_VP: return TEXT("Pure hardware");
	default:			   return TEXT("Unknown");
	}
};

/**
 *  Returns the string for the given present interval.
 */
TCHAR* gPresentIntervalToString( UINT pi )
{
	switch( pi ) {
	case D3DPRESENT_INTERVAL_IMMEDIATE: return TEXT("Immediate");
	case D3DPRESENT_INTERVAL_DEFAULT:	return TEXT("Default");
	case D3DPRESENT_INTERVAL_ONE:		return TEXT("One refresh");
	case D3DPRESENT_INTERVAL_TWO:		return TEXT("Two refreshes");
	case D3DPRESENT_INTERVAL_THREE: 	return TEXT("Three refreshes");
	case D3DPRESENT_INTERVAL_FOUR:		return TEXT("Four refreshes");
	default:							return TEXT("Unknown");
	}
};


//---------------------------------------------------------------------------

INT_PTR CALLBACK gDialogProcHelper( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
	return gSettingsDialog->dialogProc( hDlg, msg, wParam, lParam );
};



//---------------------------------------------------------------------------
// CD3DSettingsDialog
//---------------------------------------------------------------------------


CD3DSettingsDialog::CD3DSettingsDialog( const CD3DEnumeration& enumeration, const CD3DSettings& settings )
:	mEnumeration(&enumeration),
	mSettings(settings)
{
	gSettingsDialog = this;
};




/** Adds an entry to the combo box. */
void CD3DSettingsDialog::comboBoxAdd( int id, const void* pData, const TCHAR* pstrDesc )
{
	HWND hwndCtrl = GetDlgItem( mDlg, id );
	DWORD dwItem = ComboBox_AddString( hwndCtrl, pstrDesc );
	ComboBox_SetItemData( hwndCtrl, dwItem, pData );
};


/** Selects an entry in the combo box. */
void CD3DSettingsDialog::comboBoxSelect( int id, const void* pData )
{
	HWND hwndCtrl = GetDlgItem( mDlg, id );
	int count = comboBoxCount( id );
	for( int iItem = 0; iItem < count; ++iItem ) {
		if( (void*)ComboBox_GetItemData( hwndCtrl, iItem ) == pData ) {
			ComboBox_SetCurSel( hwndCtrl, iItem );
			PostMessage( mDlg, WM_COMMAND, MAKEWPARAM( id, CBN_SELCHANGE ), (LPARAM)hwndCtrl );
			return;
		}
	}
};

/** Selects an entry in the combo box. */

void CD3DSettingsDialog::comboBoxSelectIndex( int id, int index )
{
	HWND hwndCtrl = GetDlgItem( mDlg, id );
	ComboBox_SetCurSel( hwndCtrl, index );
	PostMessage( mDlg, WM_COMMAND, MAKEWPARAM( id, CBN_SELCHANGE ), (LPARAM)hwndCtrl );
};


/** Returns the data for the selected entry in the combo box. */
const void* CD3DSettingsDialog::comboBoxSelected( int id )
{
	HWND hwndCtrl = GetDlgItem( mDlg, id );
	int index = ComboBox_GetCurSel( hwndCtrl );
	if( index < 0 )
		return NULL;
	return (const void*)ComboBox_GetItemData( hwndCtrl, index );
};

/**
 *  Returns whether any entry in the combo box is selected.  This is more useful
 *  than comboBoxSelected() when you need to distinguish between having no item
 *  selected vs. having an item selected whose itemData is NULL.
 */
bool CD3DSettingsDialog::comboBoxSomethingSelected( int id )
{
	HWND hwndCtrl = GetDlgItem( mDlg, id );
	int index = ComboBox_GetCurSel( hwndCtrl );
	return ( index >= 0 );
};

/** Returns the number of entries in the combo box. */
int CD3DSettingsDialog::comboBoxCount( int id )
{
	HWND hwndCtrl = GetDlgItem( mDlg, id );
	return ComboBox_GetCount( hwndCtrl );
};

/** Clears the entries in the combo box. */
void CD3DSettingsDialog::comboBoxClear( int id )
{
	HWND hwndCtrl = GetDlgItem( mDlg, id );
	ComboBox_ResetContent( hwndCtrl );
};

/** Returns whether the combo box contains the given text. */
bool CD3DSettingsDialog::comboBoxContainsText( int id, const TCHAR* pstrText )
{
	TCHAR strItem[200];
	HWND hwndCtrl = GetDlgItem( mDlg, id );
	UINT count = comboBoxCount( id );
	for( UINT iItem = 0; iItem < count; iItem++ )
	{
		if( ComboBox_GetLBTextLen( hwndCtrl, iItem ) >= 200 )
			continue; // shouldn't happen, but don't overwrite buffer if it does
		ComboBox_GetLBText( hwndCtrl, iItem, strItem );
		if( lstrcmp( strItem, pstrText ) == 0 )
			return true;
	}
	return false;
};




/** Show the D3D settings dialog. */
INT_PTR CD3DSettingsDialog::showDialog( HINSTANCE instance, HWND hwndParent, CD3DApplication& application )
{
	gSettingsApp = &application;
	return DialogBox( instance, MAKEINTRESOURCE( IDD_SELECTDEVICE ), hwndParent, gDialogProcHelper );
};


/** Handle window messages in the dialog. */
INT_PTR CD3DSettingsDialog::dialogProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER( lParam );
	
	switch( msg ) {
	case WM_INITDIALOG:
		{
			//
			// initialize dialog

			mDlg = hDlg;
			
			// fill adapter combo box, updating the selected adapter will trigger
			// updates of the rest of the dialog.
			for( int iai = 0; iai < mEnumeration->mAdapterInfos.size(); ++iai ) {
				const SD3DAdapterInfo* adInfo = mEnumeration->mAdapterInfos[iai];
				TCHAR strDescription[512];
				dingus::convertAnsiStringToGenericCch( strDescription, adInfo->adapterID.Description, 512 );
				comboBoxAdd( IDC_ADAPTER_COMBO, adInfo, strDescription );
				if( adInfo->adapterOrdinal == mSettings.getAdapterOrdinal() )
					comboBoxSelect( IDC_ADAPTER_COMBO, adInfo );
			}
			if( !comboBoxSomethingSelected( IDC_ADAPTER_COMBO ) && comboBoxCount( IDC_ADAPTER_COMBO ) > 0 )
				comboBoxSelectIndex( IDC_ADAPTER_COMBO, 0 );

			// fill custom dialog
			gSettingsApp->customSettingsInit( hDlg );
		}
		return TRUE;
		
	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case IDOK:
			// get custom settings
			gSettingsApp->customSettingsOK( hDlg );
			EndDialog( hDlg, IDOK );
			break;
		case IDCANCEL:
			EndDialog( hDlg, IDCANCEL );
			break;
		case IDC_ADAPTER_COMBO:
			if( CBN_SELCHANGE == HIWORD(wParam) )
				adapterChanged();
			break;
		case IDC_DEVICE_COMBO:
			if( CBN_SELCHANGE == HIWORD(wParam) )
				deviceChanged();
			break;
		case IDC_ADAPTERFORMAT_COMBO:
			if( CBN_SELCHANGE == HIWORD(wParam) )
				adapterFormatChanged();
			break;
		case IDC_RESOLUTION_COMBO:
			if( CBN_SELCHANGE == HIWORD(wParam) )
				resolutionChanged();
			break;
		case IDC_REFRESHRATE_COMBO:
			if( CBN_SELCHANGE == HIWORD(wParam) )
				refreshRateChanged();
			break;
		case IDC_BACKBUFFERFORMAT_COMBO:
			if( CBN_SELCHANGE == HIWORD(wParam) )
				backBufferFormatChanged();
			break;
		case IDC_DEPTHSTENCILBUFFERFORMAT_COMBO:
			if( CBN_SELCHANGE == HIWORD(wParam) )
				depthStencilBufferChanged();
			break;
		case IDC_MULTISAMPLE_COMBO:
			if( CBN_SELCHANGE == HIWORD(wParam) )
				multiSampleTypeChanged();
			break;
		case IDC_MULTISAMPLE_QUALITY_COMBO:
			if( CBN_SELCHANGE == HIWORD(wParam) )
				multiSampleQualityChanged();
			break;
		case IDC_VERTEXPROCESSING_COMBO:
			if( CBN_SELCHANGE == HIWORD(wParam) )
				vertexProcessingChanged();
			break;
		case IDC_PRESENTINTERVAL_COMBO:
			if( CBN_SELCHANGE == HIWORD(wParam) )
				presentIntervalChanged();
			break;
		case IDC_WINDOW:
		case IDC_FULLSCREEN:
			windowedFullscreenChanged();
			break;
		}
		return TRUE;
		
		default:
			return FALSE;
	}
}


/** Respond to a change of selected adapter. */
void CD3DSettingsDialog::adapterChanged()
{
	const SD3DAdapterInfo* adInfo = (const SD3DAdapterInfo*)comboBoxSelected( IDC_ADAPTER_COMBO );
	if( adInfo == NULL )
		return;
	
	mSettings.mSettings[mSettings.mMode].adapterInfo = adInfo;
	
	// update device combo box
	comboBoxClear( IDC_DEVICE_COMBO );
	for( int idi = 0; idi < adInfo->deviceInfos.size(); ++idi ) {
		const SD3DDeviceInfo* devInfo = adInfo->deviceInfos[idi];
		comboBoxAdd( IDC_DEVICE_COMBO, devInfo, gD3DDevTypeToString( devInfo->deviceType ) );
		if( devInfo->deviceType == mSettings.getDevType() )
			comboBoxSelect( IDC_DEVICE_COMBO, devInfo );
	}
	if( !comboBoxSomethingSelected( IDC_DEVICE_COMBO ) && comboBoxCount( IDC_DEVICE_COMBO ) > 0 )
		comboBoxSelectIndex( IDC_DEVICE_COMBO, 0 );
}

/**
 *  Respond to a change of selected device by resetting the fullscreen/windowed
 *  radio buttons.  Updating these buttons will trigger updates of the rest of the dialog.
 */
void CD3DSettingsDialog::deviceChanged()
{
	const SD3DDeviceInfo* devInfo = (const SD3DDeviceInfo*)comboBoxSelected( IDC_DEVICE_COMBO );
	if( devInfo == NULL )
		return;
	
	mSettings.mSettings[mSettings.mMode].deviceInfo = devInfo;
	
	// update fullscreen/windowed radio buttons
	bool hasWindowedCombo = false;
	bool hasFullscreenCombo = false;
	for( int idc = 0; idc < devInfo->deviceCombos.size(); ++idc ) {
		const SD3DDeviceCombo* devCombo = devInfo->deviceCombos[idc];
		if( devCombo->isWindowed )
			hasWindowedCombo = true;
		else
			hasFullscreenCombo = true;
	};
	EnableWindow( GetDlgItem( mDlg, IDC_WINDOW ), hasWindowedCombo );
	EnableWindow( GetDlgItem( mDlg, IDC_FULLSCREEN ), hasFullscreenCombo );
	if( mSettings.mMode == CD3DSettings::WINDOWED && hasWindowedCombo )
		CheckRadioButton( mDlg, IDC_WINDOW, IDC_FULLSCREEN, IDC_WINDOW );
	else
		CheckRadioButton( mDlg, IDC_WINDOW, IDC_FULLSCREEN, IDC_FULLSCREEN );
	windowedFullscreenChanged();
}


/**
 *  Respond to a change of windowed/fullscreen state by rebuilding the adapter
 *  format list, resolution list, and refresh rate list. Updating the selected
 *  adapter format will trigger updates of the rest of the dialog.
 */
void CD3DSettingsDialog::windowedFullscreenChanged()
{
	const SD3DAdapterInfo* adInfo = (const SD3DAdapterInfo*)comboBoxSelected( IDC_ADAPTER_COMBO );
	const SD3DDeviceInfo* devInfo = (const SD3DDeviceInfo*)comboBoxSelected( IDC_DEVICE_COMBO );
	if( adInfo == NULL || devInfo == NULL )
		return;
	
	if( IsDlgButtonChecked( mDlg, IDC_WINDOW ) ) {

		//
		// windowed

		mSettings.mMode = CD3DSettings::WINDOWED;
		mSettings.mSettings[mSettings.mMode].adapterInfo = adInfo;
		mSettings.mSettings[mSettings.mMode].deviceInfo = devInfo;
		
		const D3DDISPLAYMODE& dm = mSettings.getDisplayMode();

		// Update adapter format combo box
		comboBoxClear( IDC_ADAPTERFORMAT_COMBO );
		comboBoxAdd( IDC_ADAPTERFORMAT_COMBO, ULongToPtr(dm.Format), dingus::convertD3DFormatToString( dm.Format ) );
		comboBoxSelectIndex( IDC_ADAPTERFORMAT_COMBO, 0 );
		EnableWindow( GetDlgItem( mDlg, IDC_ADAPTERFORMAT_COMBO ), false );
		
		// update resolution combo box
		DWORD dwResolutionData;
		TCHAR strResolution[50];
		dwResolutionData = MAKELONG( dm.Width, dm.Height );
		_sntprintf( strResolution, 50, TEXT("%d x %d"), dm.Width, dm.Height );
		strResolution[49] = 0;
		comboBoxClear( IDC_RESOLUTION_COMBO );
		comboBoxAdd( IDC_RESOLUTION_COMBO, ULongToPtr(dwResolutionData), strResolution );
		comboBoxSelectIndex( IDC_RESOLUTION_COMBO, 0 );
		EnableWindow( GetDlgItem( mDlg, IDC_RESOLUTION_COMBO ), false );
		
		// update refresh rate combo box
		TCHAR strRefreshRate[50];
		if( dm.RefreshRate == 0 )
			lstrcpy( strRefreshRate, TEXT("Default rate") );
		else
			_sntprintf( strRefreshRate, 50, TEXT("%d Hz"), dm.RefreshRate );
		strRefreshRate[49] = 0;
		comboBoxClear( IDC_REFRESHRATE_COMBO );
		comboBoxAdd( IDC_REFRESHRATE_COMBO, ULongToPtr(dm.RefreshRate),
			strRefreshRate );
		comboBoxSelectIndex( IDC_REFRESHRATE_COMBO, 0 );
		EnableWindow( GetDlgItem( mDlg, IDC_REFRESHRATE_COMBO ), false );

	} else {

		//
		// fullscreen

		mSettings.mMode = CD3DSettings::FULLSCREEN;
		mSettings.mSettings[mSettings.mMode].adapterInfo = adInfo;
		mSettings.mSettings[mSettings.mMode].deviceInfo = devInfo;
		
		const D3DDISPLAYMODE& dm = mSettings.getDisplayMode();

		// update adapter format combo box
		comboBoxClear( IDC_ADAPTERFORMAT_COMBO );
		for( int idc = 0; idc < devInfo->deviceCombos.size(); ++idc ) {
			const SD3DDeviceCombo* devCombo = devInfo->deviceCombos[idc];
			D3DFORMAT fmt = devCombo->adapterFormat;
			if( !comboBoxContainsText( IDC_ADAPTERFORMAT_COMBO, dingus::convertD3DFormatToString( fmt ) ) ) {
				comboBoxAdd( IDC_ADAPTERFORMAT_COMBO, ULongToPtr(fmt), 
					dingus::convertD3DFormatToString( fmt ) );
				if( fmt == dm.Format )
					comboBoxSelect( IDC_ADAPTERFORMAT_COMBO, ULongToPtr(fmt) );
			}
		}
		if( !comboBoxSomethingSelected( IDC_ADAPTERFORMAT_COMBO ) && comboBoxCount( IDC_ADAPTERFORMAT_COMBO ) > 0 )
			comboBoxSelectIndex( IDC_ADAPTERFORMAT_COMBO, 0 );

		EnableWindow( GetDlgItem( mDlg, IDC_ADAPTERFORMAT_COMBO), true );
		
		// update resolution combo box
		EnableWindow( GetDlgItem( mDlg, IDC_RESOLUTION_COMBO), true );
		
		// update refresh rate combo box
		EnableWindow( GetDlgItem( mDlg, IDC_REFRESHRATE_COMBO), true );
	}
}


/**
 *  Respond to a change of selected adapter format by rebuilding the resolution
 *  list and back buffer format list. Updating the selected resolution and back
 *  buffer format will trigger updates of the rest of the dialog.
 */
void CD3DSettingsDialog::adapterFormatChanged()
{
	if( !IsDlgButtonChecked( mDlg, IDC_WINDOW ) ) {

		//
		// fullscreen

		const SD3DAdapterInfo* adInfo = (const SD3DAdapterInfo*)comboBoxSelected( IDC_ADAPTER_COMBO );
		D3DFORMAT fmt = (D3DFORMAT)PtrToUlong( comboBoxSelected( IDC_ADAPTERFORMAT_COMBO ) );
		mSettings.mSettings[CD3DSettings::FULLSCREEN].displayMode.Format = fmt;

		const D3DDISPLAYMODE& fullscreenDM = mSettings.mSettings[CD3DSettings::FULLSCREEN].displayMode;
		
		comboBoxClear( IDC_RESOLUTION_COMBO );
		for( int idm = 0; idm < adInfo->displayModes.size(); ++idm ) {
			const D3DDISPLAYMODE& dm = adInfo->displayModes[idm];
			if( dm.Format == fmt ) {
				DWORD dwResolutionData;
				TCHAR strResolution[50];
				dwResolutionData = MAKELONG( dm.Width, dm.Height );
				_sntprintf( strResolution, 50, TEXT("%d x %d"), dm.Width, dm.Height );
				strResolution[49] = 0;
				if( !comboBoxContainsText( IDC_RESOLUTION_COMBO, strResolution ) ) {
					comboBoxAdd( IDC_RESOLUTION_COMBO, ULongToPtr( dwResolutionData ), strResolution );
					if( fullscreenDM.Width == dm.Width && fullscreenDM.Height == dm.Height)
						comboBoxSelect( IDC_RESOLUTION_COMBO, ULongToPtr( dwResolutionData ) );
				}
			}
		}
		if( !comboBoxSomethingSelected( IDC_RESOLUTION_COMBO ) && comboBoxCount( IDC_RESOLUTION_COMBO ) > 0 )
			comboBoxSelectIndex( IDC_RESOLUTION_COMBO, 0 );
	}
	
	// update backbuffer format combo box
	const SD3DDeviceInfo* devInfo = (const SD3DDeviceInfo*)comboBoxSelected( IDC_DEVICE_COMBO );
	if( devInfo == NULL )
		return;

	comboBoxClear( IDC_BACKBUFFERFORMAT_COMBO );
	for( int idc = 0; idc < devInfo->deviceCombos.size(); ++idc ) {
		const SD3DDeviceCombo* devCombo = devInfo->deviceCombos[idc];
		bool settingsWnd = (mSettings.mMode == CD3DSettings::WINDOWED);

		if( devCombo->isWindowed == settingsWnd && devCombo->adapterFormat == mSettings.getDisplayMode().Format ) {
			if( !comboBoxContainsText( IDC_BACKBUFFERFORMAT_COMBO, dingus::convertD3DFormatToString( devCombo->backBufferFormat ) ) ) {
				comboBoxAdd( IDC_BACKBUFFERFORMAT_COMBO, ULongToPtr(devCombo->backBufferFormat),
					dingus::convertD3DFormatToString( devCombo->backBufferFormat ) );
				if( devCombo->backBufferFormat == mSettings.getBackBufferFormat() )
					comboBoxSelect( IDC_BACKBUFFERFORMAT_COMBO, ULongToPtr(devCombo->backBufferFormat) );
			}
		}
	}
	if( !comboBoxSomethingSelected( IDC_BACKBUFFERFORMAT_COMBO ) && comboBoxCount( IDC_BACKBUFFERFORMAT_COMBO ) > 0 )
		comboBoxSelectIndex( IDC_BACKBUFFERFORMAT_COMBO, 0 );
};


/** Respond to a change of selected resolution by rebuilding the refresh rate list. */
void CD3DSettingsDialog::resolutionChanged()
{
	if( mSettings.mMode	== CD3DSettings::WINDOWED )
		return;
	
	const SD3DAdapterInfo* adInfo = (const SD3DAdapterInfo*)comboBoxSelected( IDC_ADAPTER_COMBO );
	if( adInfo == NULL )
		return;
	
	// update settings with new resolution
	DWORD dwResolutionData = PtrToUlong( comboBoxSelected( IDC_RESOLUTION_COMBO ) );
	UINT width = LOWORD( dwResolutionData );
	UINT height = HIWORD( dwResolutionData );
	mSettings.getDisplayMode().Width = width;
	mSettings.getDisplayMode().Height = height;
	
	// update refresh rate list based on new resolution
	D3DFORMAT adapterFormat = (D3DFORMAT)PtrToUlong( comboBoxSelected( IDC_ADAPTERFORMAT_COMBO ) );
	comboBoxClear( IDC_REFRESHRATE_COMBO );
	for( int idm = 0; idm < adInfo->displayModes.size(); ++idm ) {
		const D3DDISPLAYMODE& dm = adInfo->displayModes[idm];
		if( dm.Format == adapterFormat &&
			dm.Width  == width &&
			dm.Height == height )
		{
			TCHAR strRefreshRate[50];
			if( dm.RefreshRate == 0 )
				lstrcpy( strRefreshRate, TEXT("Default Rate") );
			else
				_sntprintf( strRefreshRate, 50, TEXT("%d Hz"), dm.RefreshRate );
			strRefreshRate[49] = 0;
			if( !comboBoxContainsText( IDC_REFRESHRATE_COMBO, strRefreshRate ) ) {
				comboBoxAdd( IDC_REFRESHRATE_COMBO, UlongToPtr( dm.RefreshRate ), strRefreshRate );
				if( mSettings.getDisplayMode().RefreshRate == dm.RefreshRate )
					comboBoxSelect( IDC_REFRESHRATE_COMBO, UlongToPtr( dm.RefreshRate ) );
			}
		}
	}
	if( !comboBoxSomethingSelected( IDC_REFRESHRATE_COMBO ) && comboBoxCount( IDC_REFRESHRATE_COMBO ) > 0 )
		comboBoxSelectIndex( IDC_REFRESHRATE_COMBO, 0 );
}


/** Respond to a change of selected refresh rate. */
void CD3DSettingsDialog::refreshRateChanged()
{
	if( mSettings.mMode	== CD3DSettings::WINDOWED )
		return;
	
	// update settings with new refresh rate
	int refreshRate = PtrToUlong( comboBoxSelected( IDC_REFRESHRATE_COMBO ) );
	mSettings.getDisplayMode().RefreshRate = refreshRate;
}


/**
 *  Respond to a change of selected back buffer format by rebuilding the
 *  depth/stencil format list, multisample type list, and vertex processing list.
 */
void CD3DSettingsDialog::backBufferFormatChanged()
{
	const SD3DDeviceInfo* devInfo = (const SD3DDeviceInfo*)comboBoxSelected( IDC_DEVICE_COMBO );
	D3DFORMAT adapterFormat = (D3DFORMAT)PtrToUlong( comboBoxSelected( IDC_ADAPTERFORMAT_COMBO ) );
	D3DFORMAT backBufferFormat = (D3DFORMAT)PtrToUlong( comboBoxSelected( IDC_BACKBUFFERFORMAT_COMBO ) );
	if( devInfo == NULL )
		return;
	
	for( int idc = 0; idc < devInfo->deviceCombos.size(); ++idc ) {
		const SD3DDeviceCombo* devCombo = devInfo->deviceCombos[idc];
		bool settingsWnd = (mSettings.mMode == CD3DSettings::WINDOWED);
		if( devCombo->isWindowed == settingsWnd &&
			devCombo->adapterFormat == adapterFormat &&
			devCombo->backBufferFormat == backBufferFormat )
		{
			mSettings.mSettings[mSettings.mMode].deviceCombo = devCombo;
			
			comboBoxClear( IDC_DEPTHSTENCILBUFFERFORMAT_COMBO );
			if( mEnumeration->mUsesDepthBuffer ) {
				for( int ifmt = 0; ifmt < devCombo->depthStencilFormats.size(); ++ifmt ) {
					D3DFORMAT fmt = (D3DFORMAT)devCombo->depthStencilFormats[ifmt];
					comboBoxAdd( IDC_DEPTHSTENCILBUFFERFORMAT_COMBO, ULongToPtr(fmt), 
						dingus::convertD3DFormatToString(fmt) );
					if( fmt == mSettings.getDepthStencilFormat() )
						comboBoxSelect( IDC_DEPTHSTENCILBUFFERFORMAT_COMBO, ULongToPtr(fmt) );
				}
				if( !comboBoxSomethingSelected( IDC_DEPTHSTENCILBUFFERFORMAT_COMBO ) && 
					comboBoxCount( IDC_DEPTHSTENCILBUFFERFORMAT_COMBO ) > 0 )
				{
					comboBoxSelectIndex( IDC_DEPTHSTENCILBUFFERFORMAT_COMBO, 0 );
				}
			} else {
				EnableWindow( GetDlgItem( mDlg, IDC_DEPTHSTENCILBUFFERFORMAT_COMBO ), false );
				comboBoxAdd( IDC_DEPTHSTENCILBUFFERFORMAT_COMBO, NULL, TEXT("(not used)") );
				comboBoxSelectIndex( IDC_DEPTHSTENCILBUFFERFORMAT_COMBO, 0 );
			}
			
			comboBoxClear( IDC_VERTEXPROCESSING_COMBO );
			for( int ivpt = 0; ivpt < devCombo->vertexProcessings.size(); ++ivpt ) {
				eVertexProcessing vpt = (eVertexProcessing)devCombo->vertexProcessings[ivpt];
				comboBoxAdd( IDC_VERTEXPROCESSING_COMBO, ULongToPtr(vpt), gVertexProcessingToString(vpt) );
				if( vpt == mSettings.getVertexProcessing() )
					comboBoxSelect( IDC_VERTEXPROCESSING_COMBO, ULongToPtr(vpt) );
			}
			if( !comboBoxSomethingSelected( IDC_VERTEXPROCESSING_COMBO ) && 
				comboBoxCount( IDC_VERTEXPROCESSING_COMBO ) > 0 )
			{
				comboBoxSelectIndex( IDC_VERTEXPROCESSING_COMBO, 0 );
			}
			
			comboBoxClear( IDC_PRESENTINTERVAL_COMBO );
			for( int ipi = 0; ipi < devCombo->presentIntervals.size(); ++ipi ) {
				int pi = devCombo->presentIntervals[ipi];
				comboBoxAdd( IDC_PRESENTINTERVAL_COMBO, UlongToPtr( pi ), gPresentIntervalToString(pi) );
				if( pi == mSettings.getPresentInterval() )
					comboBoxSelect( IDC_PRESENTINTERVAL_COMBO, UlongToPtr( pi ) );
			}
			if( !comboBoxSomethingSelected( IDC_PRESENTINTERVAL_COMBO ) && 
				comboBoxCount( IDC_PRESENTINTERVAL_COMBO ) > 0 )
			{
				comboBoxSelectIndex( IDC_PRESENTINTERVAL_COMBO, 0 );
			}
			
			break;
		}
	}
}

/** Respond to a change of selected depth/stencil buffer format. */
void CD3DSettingsDialog::depthStencilBufferChanged()
{
	D3DFORMAT fmt = (D3DFORMAT)PtrToUlong( comboBoxSelected( IDC_DEPTHSTENCILBUFFERFORMAT_COMBO ) );
	if( mEnumeration->mUsesDepthBuffer )
		mSettings.setDepthStencilFormat( fmt );
	
	// build multisample list
	const SD3DDeviceCombo& devCombo = mSettings.getDeviceCombo();
	comboBoxClear( IDC_MULTISAMPLE_COMBO );
	for( int ims = 0; ims < devCombo.multiSampleTypes.size(); ++ims ) {
		D3DMULTISAMPLE_TYPE msType = (D3DMULTISAMPLE_TYPE)devCombo.multiSampleTypes[ims];
		
		// check for DS/MS conflicts
		bool conflictFound = false;
		for( int iconf = 0; iconf < devCombo.conflicts.size(); ++iconf ) {
			const SD3DDeviceCombo::SDSMSConflict& conf = devCombo.conflicts[iconf];
			if( conf.format == fmt && conf.type == msType ) {
				conflictFound = true;
				break;
			}
		}
		if( !conflictFound ) {
			comboBoxAdd( IDC_MULTISAMPLE_COMBO, ULongToPtr(msType), gMultiSampleTypeToString(msType) );
			if( msType == mSettings.getMultiSampleType() )
				comboBoxSelect( IDC_MULTISAMPLE_COMBO, ULongToPtr(msType) );
		}
	}
	if( !comboBoxSomethingSelected( IDC_MULTISAMPLE_COMBO ) && comboBoxCount( IDC_MULTISAMPLE_COMBO ) > 0 )
		comboBoxSelectIndex( IDC_MULTISAMPLE_COMBO, 0 );
}


/** Respond to a change of selected multisample type. */
void CD3DSettingsDialog::multiSampleTypeChanged()
{
	D3DMULTISAMPLE_TYPE mst = (D3DMULTISAMPLE_TYPE)PtrToUlong( comboBoxSelected( IDC_MULTISAMPLE_COMBO ) );
	mSettings.setMultiSampleType( mst );
	
	// set up max quality for this mst
	const SD3DDeviceCombo& devCombo = mSettings.getDeviceCombo();
	DWORD maxQuality = 0;
	
	for( int ims = 0; ims < devCombo.multiSampleTypes.size(); ++ims ) {
		D3DMULTISAMPLE_TYPE msType = (D3DMULTISAMPLE_TYPE)devCombo.multiSampleTypes[ims];
		if( msType == mst ) {
			maxQuality = devCombo.multiSampleQualities[ims];
			break;
		}
	}
	
	comboBoxClear( IDC_MULTISAMPLE_QUALITY_COMBO );
	for( int msq = 0; msq < maxQuality; msq++ ) {
		TCHAR str[100];
		wsprintf( str, TEXT("%d"), msq );
		comboBoxAdd( IDC_MULTISAMPLE_QUALITY_COMBO, UlongToPtr( msq ), str );
		if( msq == mSettings.getMultiSampleQuality() )
			comboBoxSelect( IDC_MULTISAMPLE_QUALITY_COMBO, UlongToPtr( msq ) );
	}
	if( !comboBoxSomethingSelected( IDC_MULTISAMPLE_QUALITY_COMBO ) && comboBoxCount( IDC_MULTISAMPLE_QUALITY_COMBO ) > 0 )
		comboBoxSelectIndex( IDC_MULTISAMPLE_QUALITY_COMBO, 0 );
}


/** Respond to a change of selected multisample quality. */
void CD3DSettingsDialog::multiSampleQualityChanged( void )
{
	DWORD msq = (DWORD)PtrToUlong( comboBoxSelected( IDC_MULTISAMPLE_QUALITY_COMBO ) );
	mSettings.setMultiSampleQuality( msq );
}


/** Respond to a change of selected vertex processing type. */
void CD3DSettingsDialog::vertexProcessingChanged( void )
{
	eVertexProcessing vpt = (eVertexProcessing)PtrToUlong( comboBoxSelected( IDC_VERTEXPROCESSING_COMBO ) );
	mSettings.setVertexProcessing( vpt );
}


/** Respond to a change of selected present interval. */
void CD3DSettingsDialog::presentIntervalChanged( void )
{
	UINT pi = PtrToUlong( comboBoxSelected( IDC_PRESENTINTERVAL_COMBO ) );
	mSettings.setPresentInterval( pi );
}

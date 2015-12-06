/* 
   OpOrbiterWindows.h

   2005, Oliver Pieper (pieper@viaregio.de)

   Perform various tasks on Windows GUI elements.
   The purpose of this module is mainly to keep the source code
   in my Orbiter projects more readable.
*/

bool IsButtonPushed(HWND hDlg, int nIDDlgItem) {
	return (hDlg && (SendDlgItemMessage(hDlg, nIDDlgItem, BM_GETSTATE, 0, 0) & BST_PUSHED));
}


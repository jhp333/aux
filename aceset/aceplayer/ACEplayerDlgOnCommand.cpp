#include "aceplayer.h"
#include "ACEplayerDlg.h"

void CACEplayerDlg::OnCommand(int idc, HWND hwndCtl, UINT event)
{
	static bool acoustmix(false);
	CStdString outStr, str, str2, tempFName;
	DWORD gle;
	char szbuffer[MAX_PATH], errstr[128];
	char fname[MAX_PATH], fullfname[MAX_PATH], estr[256];
	ACESEQ seq;

	switch(idc)
	{
	case ID_VIEW_MATLABMESSAGES:
		ShowWindow(hMATLABMSG, SW_SHOW);
		SetWindowText(hMATLABMSG, "MATLAB message");
		break;

	case ID_VIEW_PIPE:
		ShowWindow(hPipeLog, SW_SHOW);
		break;

	case ID_VIEW_PULSESTAT:
		ShowWindow(hPulseView, SW_SHOW);
		break;

	case ID_FILE_OPENMAPA:
		hACEobj[0].EVK_OpenMAP();
		break;
	case ID_FILE_OPENMAPB:
		hACEobj[1].EVK_OpenMAP();
		break;
	case IDC_SWAP_AB:
		swapAB();
		break;
	case IDC_CLOSE_B:
		hACEobj[1].EVK_CloseMAP();
		param2 = hACEobj[1].param;
		break;
	case IDC_OPEN_WAVE:
		fname[0]='\0';fullfname[0]='\0';
		if (!fdlg.FileOpenDlg(fullfname, fname, "WAVE Files (*.WAV)\0*.wav\0", "wav"))
		{
			gle = GetLastError();
			if (gle!=0)
			{
				sprintf(errstr, "GetOpenFileName failed. code=%d", gle);
				MessageBox (errstr, "aceplayer", MB_OK);	
			}
		}
		else
			SendDlgItemMessage (IDC_WAVE_LIST, LB_ADDSTRING, 0, (LPARAM)fullfname);
		break;

	case IDC_REMOVE:
		SendDlgItemMessage(IDC_WAVE_LIST, LB_DELETESTRING, SendDlgItemMessage(IDC_WAVE_LIST, LB_GETCURSEL, 0, 0), 0);
		break;

	case IDC_MAPA:
		SetDlgItemInt(IDC_ATTEN_A, 0, 0);
		SetDlgItemText(IDC_ATTEN_B, "all");
		break;
	case IDC_MAPB:
		SetDlgItemInt(IDC_ATTEN_B, 0, 0);
		SetDlgItemText(IDC_ATTEN_A, "all");
		break;

	case IDC_WAVE_LIST:
		::SendMessage (hStatusbar, SB_SETTEXT, 1, (LPARAM)"");
		if (event == LBN_DBLCLK) // Prepare and Play (stream) current file
		{
			EnableDlgItem(hDlg, IDC_WAVE_LIST, 0);
			SendDlgItemMessage(IDC_WAVE_LIST, LB_GETTEXT, SendDlgItemMessage(IDC_WAVE_LIST, LB_GETCURSEL, 0, 0), (LPARAM)fullfname);
			if (!GetACEparams(errstr)) 
			{	
				MessageBox (errstr, "", MB_OK); 
				EnableDlgItem(hDlg, IDC_WAVE_LIST, 1);
				break; 
			}
			// Noise addition for simple-play done here (if the prepare command is from the controller, the noise addition was taken care of there)

			str.Format("WAV=wave(\"%s\")", fullfname);
			try {
				AstSig.Reset(FS);
				AstSig.SetNewScript(str.c_str());
				AstSig.Compute();
			} catch (const char *errmsg) {
				MessageBox(strchr(errmsg,':')+1, "WAV initialization");
				EnableDlgItem(hDlg, IDC_WAVE_LIST, 1);
				break;
			}

			//evaluate the signal to present
			if (PrepareAUX())
			{
				FulfillFile(str, AppPath, "temp.wav");
				//signal ready. 
				if (!Sig.Wavwrite(str.c_str(), estr))
				{
					MessageBox (estr);
					break;
				}
				::SendMessage (hStatusbar, SB_SETTEXT, 1, (LPARAM)"Preparing");
				if (!Prepare(&seq, str.c_str(), duration, outStr))
				{
					strcpy(szbuffer, "Error in PREPARE");
					MessageBox (outStr, szbuffer, MB_OK);
					::SendMessage (hStatusbar, SB_SETTEXT, 1, (LPARAM)outStr.c_str());
				}
				else
				{
					sprintf(szbuffer, "Prepared, duration=%5.1f s", duration);
					::SendMessage (hStatusbar, SB_SETTEXT, 1, (LPARAM)szbuffer);
					sprintf(szbuffer, "Presenting... duration=%5.1f s", duration);
					::SendMessage (hStatusbar, SB_SETTEXT, 1, (LPARAM)szbuffer);
					if (!Present(&seq, outStr))
					{
						strcpy(szbuffer, "Error in PRESENT");
						MessageBox (outStr, szbuffer, MB_OK);
					}
					sprintf(szbuffer, "Presenting... duration=%5.1f s   done", duration);
					::SendMessage (hStatusbar, SB_SETTEXT, 1, (LPARAM)szbuffer);
				}
			} 
			EnableDlgItem(hDlg, IDC_WAVE_LIST, 1);
		}
		break;

	case IDCANCEL:
//		SendMessage(WM_CLOSE, 0, 0);
		break;
	}
}

bool CACEplayerDlg::PrepareAUX()
{
	CStdString longbuf;
	GetDlgItemText(IDC_AUX, longbuf);
	try {
		AstSig.SetNewScript(longbuf.c_str());
		Sig = AstSig.Compute();
	} catch (const char *errmsg) {
		MessageBox(errmsg, "AUX Processing", MB_ICONERROR);
		return false;
	}
	return true;
}


void CACEplayerDlg::DisplayPulses(ACESEQ *seq, const char* inputlevelstr)
{
	int minRef, maxRef, sum, count;
	char *indexDone = new char[seq->length];
	double meancl;
	FillMemory(indexDone, seq->length, 0xff);
	::SetDlgItemText(hPulseView, IDC_PULSTAT,"");
	double bigsum=0.;
	int cumcount=0;
	if (strlen(param->addmlines)>0)		
		EditPrintf (::GetDlgItem(hPulseView, IDC_PULSTAT),"(A)%s\r\n----------\r\n", param->addmlines);
	if (param->ptParam2!=NULL && strlen(param->ptParam2->addmlines)>0)		
		EditPrintf (::GetDlgItem(hPulseView, IDC_PULSTAT),"(B)%s\r\n----------\r\n", param->ptParam2->addmlines);
	for (int e=22; e>=0; e--)
	{
		count=0;
		minRef = 9999; maxRef=-9999; sum=0;
		for (int i=0; i<seq->length; i++)
		{
			while (indexDone[i]>=0) i++;
			if (i>=seq->length) break;
			// first check if electrode is e
			if (seq->ae[i]==e)
			{
				int cl = (int)seq->cl[i];
				indexDone[i] = e;
				sum += cl;
				count++;
				if (cl<minRef) minRef = cl;
				else if (cl>maxRef) maxRef = cl;
			}
		}
		if (count>0)
		{
			meancl = (double)sum/(double)count;
			EditPrintf (::GetDlgItem(hPulseView, IDC_PULSTAT), "%d\t%d\t%d\t%d\t%5.1f\r\n", e, count, minRef, maxRef, meancl);
			bigsum += meancl*count;
			cumcount += count;
		}
		else
			EditPrintf (::GetDlgItem(hPulseView, IDC_PULSTAT), "%d\t%d\t---\t---\t----\r\n", e, count);
	}
	EditPrintf (::GetDlgItem(hPulseView, IDC_PULSTAT), "\r\nWeighted Average = %4.1f\r\n", bigsum/(double)cumcount);
	EditPrintf (::GetDlgItem(hPulseView, IDC_PULSTAT), "\r\nInput RMS = %s dBSPL\r\n", inputlevelstr);
	delete[] indexDone;
}
#include "Echo.h"
#include "EchoMainWnd.h"
#include "Settings.h"
#include <time.h>

void CEchoMainWnd::OnCommand(int idc, HWND hwndCtl, UINT event) 
{
	char buf[2048], buf2[2048], date[16], time[16], errstr[512];
	char *temp[3], zbuf1[256], zbuf2[256], zbuf3[256];
	int i, res, j;
	unsigned long hThread4Pipe;
	string msg;

	switch(idc)
	{
	case IDC_RETRY:
		PlayNext();
		break;

	case IDC_PRESENTER_SET:
		GetDlgItemText (IDC_PRESENT_CONTROL, buf, sizeof(buf));
		sprintf (buf2, "SET %s", buf);
		batch->execute(buf2, 0);
		break;

	case IDC_SINGLE:
		nConc = 1;
		EnableDlgItem(hDlg, IDC_GAP, 0);
		EnableDlgItem(hDlg, IDC_PRACTICE, 1);
		SendMessage (WM_COMMAND, IDC_STOP); 
		break;
	case IDC_DUAL:
		nConc = 2;
		EnableDlgItem(hDlg, IDC_GAP, 1);
		EnableDlgItem(hDlg, IDC_PRACTICE, 0);
		SendMessage (WM_COMMAND, IDC_STOP); 
		break;

	case ID_HELP_ECHO:
		//HtmlHelp(NULL, "echo.chm::/whatisecho.html", HH_DISPLAY_TOPIC, 0);
		break;

	case IDC_CONS:
	case IDC_GENERAL:
		if (SendDlgItemMessage (IDC_CONS, BM_GETCHECK) == BST_CHECKED)
		{
			consTesting = 1;
			for (i=IDS_TOKEN1; i<=IDS_TOKEN22; i++)
				ShowWindow (GetDlgItem(IDC_TOKEN+i), SW_SHOW); 
			ShowWindow (GetDlgItem(IDC_GENERAL_INST1), SW_HIDE); 
			ShowWindow (GetDlgItem(IDC_GENERAL_INST2), SW_HIDE); 
			ShowWindow (GetDlgItem(IDC_GENERAL_LIST), SW_HIDE);
		}
		else
		{
			consTesting = 0;
			for (i=IDS_TOKEN1; i<=IDS_TOKEN22; i++)
				ShowWindow (GetDlgItem(IDC_TOKEN+i), SW_HIDE); 
			ShowWindow (GetDlgItem(IDC_GENERAL_INST1), SW_SHOW);
			ShowWindow (GetDlgItem(IDC_GENERAL_INST2), SW_SHOW); 
			ShowWindow (GetDlgItem(IDC_GENERAL_LIST), SW_SHOW); 
		}
		break;

	case IDC_GENERAL_LIST:
		if (event==EN_KILLFOCUS)
			PostMessage(WM__TOKENS, 0, READ_TOKENS_GENERAL);
		break;

	case IDC_WAV_DIRECTORY:
		if (event==EN_KILLFOCUS)
		{
			GetDlgItemText (IDC_WAV_DIRECTORY, ss->wavePath, sizeof(ss->wavePath));
			if (strlen(ss->wavePath)>0)
				PostMessage(WM__ORG_SESS_SCREEN);
		}
		break;

	case IDC_CONNECT_PRESENTER:
		hThread4Pipe = _beginthread(thread4pipe, 0, (void*)hDlg);
		break;

	case IDC_OPEN_SESSION:
	case IDM_FILE_OPENSESSION:
		if (SessionFileOpenDlg (hDlg, sessFileNameWPath, sessFileName))
		{
			SendMessage (WM__OPEN_SESSION, (WPARAM)sessFileNameWPath, 0);
		}
		break;

	case IDC_SAVE_SESSION:
	case IDM_FILE_SAVESESSION:
		if (SessionFileSaveDlg (hDlg, sessFileNameWPath, sessFileName))
		{
			if (WriteSessFile (errstr)!=0)
				MessageBox ("File saving error", errstr, MB_OK);
			else
				SetDlgItemText (IDC_SESSION_FNAME, sessFileNameWPath);
		}
		break;

	case IDM_FILE_SETTINGS:
		{
			CSettings dlg(this);
			dlg.DoModal();
		}
		break;

	case ID_MENUITEM40005:
		ShowWindow (hPipeMsgViewDlg,1);
		break;

	case IDM_BATCH:
		ShowWindow (batch->hDlg,1);
		break;	
	case IDM_MULTIPLEX:
		ShowWindow (hMulti->hDlg,1);
		break;

	case IDC_NAMERULE:
		if (event==EN_UPDATE)
		{
			GetDlgItemText (idc, buf, sizeof(buf));
			if (strcmp(buf,ss->nameRule)) 
			{
				EnableDlgItem(hDlg, IDC_UPDATE, 1);
				strcpy(ss->nameRule, buf);
			}
			else
				EnableDlgItem(hDlg, IDC_UPDATE, 0);
		}
		break;
	case IDC_UPDATE:
		if (event==BN_CLICKED)
		{
			DecodeNameRuleAndGenerateList (ss->nameRule, waveNameList, wavID2tokenID, ss);
			ss->nCombo = FillDesireWavList (IDC_WAVELIST2, IDC_NUM_COMBO);
		}
		break;
	case IDC_WAVREADY:
		if (event==EN_SETFOCUS)
		{
			if (strlen(missingFiles)>0)
			{
				::SetDlgItemText (hMissingDlg, IDC_MISSINGFILES, missingFiles);
				ShowWindow (hMissingDlg, 1);
			}
		}
		break;

	case IDC_BEGIN:
		if ((res = GetNConcfromProcStr(buf))>0) nConc=res;
		else {		MessageBox (buf, "", MB_OK);		break; }
		// if HIWORD is USR_MULTI, indices for addproc and preset are recorded with presentOrder.
		PresenterInfos.clear();
		if (hResult->hDlg!=NULL)
			DestroyWindow (hResult->hDlg);
		nTrials = GetDlgItemInt (IDC_NUM_TRIALS , &i);
		if (nTrials*i==0)
		{
			MessageBox ("Type Number of trials before begin", "Echo", MB_OK);
			break;
		}
		if (!wavListReady)
		{
			if (MessageBox ("All the specified wav files are not available in the path", "Continue?", MB_YESNO)==IDNO)
			break;
		}
		if (event==USR_MULTI)
		{
			multi=1;
			nTrials *= hMulti->nPresets * hMulti->nAddprocs;
		}
		else
			multi=0;

		AstSig.Reset(Fs);
		timeStamp(date, time);
		sprintf (buf, "Echo %s, session begins %s, %s\r\n", versionString, date, time);
		sessionBeganTime = buf;
		SetWindowText (hRespLog, "");
		EnableDlgItem (hDlg, IDC_BEGIN, FALSE);
		EnableDlgItem (hDlg, IDC_STOP, 1);
		
		feedBackOn = (SendDlgItemMessage (IDC_FEEDBACK, BM_GETCHECK, 0, 0)==BST_CHECKED) ? 1 : 0;
		fSingleResponse = (SendDlgItemMessage (IDC_1RESPONSE, BM_GETCHECK, 0, 0)==BST_CHECKED) ? 1 : 0;
		fAllowSameToken = (SendDlgItemMessage (IDC_ALLOWSAMETOKEN, BM_GETCHECK, 0, 0)==BST_CHECKED) ? 1 : 0;

		idPresets = new unsigned char[nTrials];
		idAddProcs = new unsigned char[nTrials];
		presentOrder = (int *) calloc (nTrials, sizeof(int));
		for (i=0; i<nConc; i++)
			respCollected[i] = (unsigned char*) calloc (nTrials, sizeof(char));
		trial = nResponse = 0;
		GenerateRandomList ();
		sprintf(buf, "Wave file combinations : %d", Combs.size());
		msg = "(FYI, " + string(buf) + ")\r\n\r\n";
		msg += "Enter the subject ID:";
		buf[0] = '\0';
		InputBox("Echo - New session", msg.c_str(), buf, sizeof(buf), false, hDlg);
		sessionSubjectID = buf;

		//Fill the Play list
		appendLine2File(LOG_FILE, "==========================");
		sprintf (buf, "Echo %s, %s, %s Prsentation Order:", versionString, date, time);
		appendLine2File(LOG_FILE, buf);
		while (SendDlgItemMessage (IDC_PLAYLIST, LB_DELETESTRING, 0, 0)!=LB_ERR) {}
		for (i=0; i<nTrials; i++)
		{
			sprintf (buf, "%d: ", i+1);
			for (j=0; j<nConc; j++)
			{
				strcat (buf, waveNameList[Combs[presentOrder[i]][j]].c_str());
				strcat (buf, " ");
			}
			if (multi)
			{
				sprintf(buf2, " %d %d", idPresets[i], idAddProcs[i]);
				strcat (buf, buf2);
			}
			SendDlgItemMessage (IDC_PLAYLIST, LB_ADDSTRING, 0, (LPARAM)buf);
			appendLine2File(LOG_FILE, buf);
		}
		BeginSessionTransmission();
		break;

	case IDC_PRACTICE:
		if ((res = GetNConcfromProcStr(buf))>0) nConc=res;
		else {		MessageBox (buf, "", MB_OK);		break; }
		AstSig.Reset(Fs);

		acousticOn = (SendDlgItemMessage (IDC_ACOUSTIC, BM_GETCHECK, 0, 0)==BST_CHECKED)  ? 1 : 0;
		EnableDlgItem (hDlg, IDC_PRACTICE, FALSE);
		EnableDlgItem (hDlg, IDC_BEGIN, FALSE);
		EnableDlgItem (hDlg, IDC_STOP, 1);
		LoadString (hInst, IDS_SUBJ_INST5, buf, sizeof(buf));
		flySendText (FL_INIT_PRAC, buf);
		flySendBin (FL_INIT_NUMBER_TOKENS, &ss->nToken, sizeof(ss->nToken));
		for (i=0; i< ss->nToken; i++)
		{
			sprintf (buf, "%s %s", ss->token[i], ss->tokenExample[i]);
			res=flySendText (FL_INIT_TOKEN_STRING, buf);
		}
		cleanpracPlayList=0;
		//NECESSARY TO AVOID CRASH UPON IDC_STOP--NO SERIOUS MEANING
		idPresets = new unsigned char[1];
		idAddProcs = new unsigned char[1];
		presentOrder = (int *) calloc (1, sizeof(int));
		respCollected[0] = (unsigned char*) calloc (1, sizeof(char));
		break;

	case IDC_PLAYLIST:
		if (event==LBN_DBLCLK)
		{
			if ((res = GetNConcfromProcStr(buf))>0) nConc=res;
			else {		MessageBox (buf, "", MB_OK);		break; }

			res= SendDlgItemMessage(IDC_PLAYLIST, LB_GETCURSEL, 0, 0);
			j = SendDlgItemMessage(IDC_PLAYLIST, LB_GETTEXT, (WPARAM)res, (LPARAM)buf);
			if (j==LB_ERR || j==0) 
			{
				MessageBox (errstr, "Empty line", MB_OK);
				break;
			}
			temp[0] = zbuf1; temp[1] = zbuf2; temp[2] = zbuf3;
			if (str2strarray(temp, 2, buf, ":")<2)
			{	MessageBox (errstr, "Invalid format for the list", MB_OK);
				break;		}
			if (!ReadLineandWav(temp[1], errstr)) 	{	MessageBox(errstr);		break;	}
			if (!PlayStim()) break;
			EnableWindow (GetDlgItem(IDC_PLAYLIST), 0);
			UpdateWindow(hDlg); //Amazing, after updating to mclmcrrt73, this is necessary, without it, the screen doesn't get updated from enable to disabled.
			//CopyEchoStimsIfNecessary(buf);
			EnableDlgItem (hDlg, IDC_PLAYLIST, 1);
		}
		break;

	case IDC_STOP:
		if (trial!=-1 && MessageBox("Are you sure you want to stop the session?", "Echo", MB_YESNO | MB_ICONQUESTION)!=IDYES)
			return;
		if (hwndCtl!=NULL)
		{
			flySendText (FL_SESSION_STOP, "");
			CleanUpTestingSession();
		}
		ShowWindow (GetDlgItem(IDC_RETRY), SW_HIDE);
		ShowWindow (GetDlgItem(IDC_BEGIN), SW_SHOW);
		EnableDlgItem (hDlg, IDC_BEGIN, TRUE);
		EnableDlgItem (hDlg, IDC_PRACTICE, TRUE);
		EnableDlgItem (hDlg, idc, FALSE);
		EnableDlgItem(hMulti->hDlg, IDC_GO, 1);
		EditPrintf (hRespLog, "\r\n");
		cleanpracPlayList=1;
		trial = -1;
		break;

	case IDC_TALK:
		if (event==EN_UPDATE)
		{
			GetDlgItemText (idc, buf, 64);
			res=flySendText (FL_TEXT, buf);
		}
		break;

	case IDM_FILE_EXIT:
		PostMessage(WM_CLOSE);
		break;

	default:
		if ( IsBetween(idc, IDC_TOKEN_B, IDC_TOKEN_TH) )
		{
			SendMessage (WM__TOKENS, 0, READ_TOKENS);
			CheckWavFilesReadyAndDisplay (IDC_WAVREADY, waveNameList, missingFiles);
		}
	}

}




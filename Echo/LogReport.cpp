#include "LogReport.h"
#include "audfret.h"
#include <string>
#include <process.h>
#include <curl/curl.h>

using namespace std;

#define POST_URL	"http://www.auditorypro.com/explog/post.php"
#define POST_KEY	"explog123"

void uploadLogFile(void *p);


LogReport::LogReport(char *namePrefix)
: NamePrefix(namePrefix), fUseEmail(false), fUseFtp(false)
{
}


void LogReport::Upload(char *fname)
{
	filename = fname;
	_beginthread(uploadLogFile, 0, this);
}


void LogReport::ReadINI(char *iniFilename)
{
	char buf[256], errStr[256];

	if (ReadItemFromINI(errStr, iniFilename, "LOG EMAIL ADDRESS", buf, sizeof(buf))>0) {
		fUseEmail = true;
		email_address = buf;
	} else
		fUseEmail = false;
	if (ReadItemFromINI(errStr, iniFilename, "LOG FTP ADDRESS", buf, sizeof(buf))>0) {
		fUseFtp = true;
		ftp_address = buf;
		if (ReadItemFromINI(errStr, iniFilename, "LOG FTP ID", buf, sizeof(buf))>0)
			ftp_id = buf;
		if (ReadItemFromINI(errStr, iniFilename, "LOG FTP PASSWORD", buf, sizeof(buf))>0)
			ftp_password = buf;
	} else
		fUseFtp = false;
}


void LogReport::WriteINI(char *iniFilename)
{
	char errStr[256];

	if (fUseEmail)
		printfINI(errStr, iniFilename, "LOG EMAIL ADDRESS", "%s", email_address);
	else
		printfINI(errStr, iniFilename, "LOG EMAIL ADDRESS", "");
	if (fUseFtp) {
		printfINI(errStr, iniFilename, "LOG FTP ADDRESS", "%s", ftp_address);
		printfINI(errStr, iniFilename, "LOG FTP ID", "%s", ftp_id);
		printfINI(errStr, iniFilename, "LOG FTP PASSWORD", "%s", ftp_password);
	} else {
		printfINI(errStr, iniFilename, "LOG FTP ADDRESS", "");
		printfINI(errStr, iniFilename, "LOG FTP ID", "");
		printfINI(errStr, iniFilename, "LOG FTP PASSWORD", "");
	}
}


size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
	size_t realsize = size * nmemb;
	string *mem = (string *)data;
	*mem += string((char *)ptr, realsize);
	return realsize;
}


void uploadLogFile(void *pLogReport)
{
	LogReport *p = (LogReport *)pLogReport;
	if (!p->fUseFtp && !p->fUseEmail)
		return;

	CURL *curl;
	CURLcode res;
	CString rfname;
	char errbuf[CURL_ERROR_SIZE] = "", buf[1000] = "";
	DWORD cbComputerName = sizeof(buf);
	GetComputerName(buf, &cbComputerName);
	rfname = p->NamePrefix + "_" + buf;

	time_t time0 = time(NULL);
	strftime(buf, sizeof(buf), "_%Y_%m_%d_%H%M.log", localtime(&time0));
	rfname += buf;

	curl_global_init(CURL_GLOBAL_ALL);
	if (p->fUseFtp) {
		if (p->ftp_address.Left(6).MakeLower() != "ftp://")
			p->ftp_address.Insert(0, "ftp://");
		if (p->ftp_address.Right(1) != "/")
			p->ftp_address += "/";
		if (FILE *file = fopen(p->filename, "rb")) {
			CString FtpURL = p->ftp_address + rfname;
			curl = curl_easy_init();
			curl_easy_setopt(curl, CURLOPT_UPLOAD, 1) ;
			curl_easy_setopt(curl, CURLOPT_USERPWD, p->ftp_id + ":" + p->ftp_password) ;
				curl_easy_setopt(curl, CURLOPT_URL, FtpURL);
			curl_easy_setopt(curl, CURLOPT_READDATA, file);
			curl_easy_setopt(curl, CURLOPT_READFUNCTION, fread);
			curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
			fclose(file);
			if (res && errbuf[0])
				MessageBox(NULL, errbuf, "Log report - FTP error", MB_OK | MB_ICONEXCLAMATION);
		} else
			MessageBox(NULL, "File open failed - "+p->filename, "Log report", MB_OK | MB_ICONEXCLAMATION);
	}

	if (p->fUseEmail) {
		string outstr;
		curl = curl_easy_init();
		struct curl_httppost* post = NULL;
		struct curl_httppost* last = NULL;
		curl_formadd(&post, &last, CURLFORM_PTRNAME, "key",
							CURLFORM_PTRCONTENTS, POST_KEY, CURLFORM_END); 
		curl_formadd(&post, &last, CURLFORM_PTRNAME, "to",
							CURLFORM_PTRCONTENTS, p->email_address, CURLFORM_END); 
		curl_formadd(&post, &last, CURLFORM_PTRNAME, "subject",
							CURLFORM_PTRCONTENTS, rfname, CURLFORM_END); 
		curl_formadd(&post, &last, CURLFORM_COPYNAME, "body",
							CURLFORM_FILECONTENT, p->filename, CURLFORM_END);
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
		curl_easy_setopt(curl, CURLOPT_URL, POST_URL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&outstr);
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
		res = curl_easy_perform(curl);
		curl_formfree(post);
		curl_easy_cleanup(curl);
		if (res && errbuf[0])
			MessageBox(NULL, errbuf, "Log report - E-Mail error", MB_OK | MB_ICONEXCLAMATION);
		else if (!outstr.empty())
			MessageBox(NULL, outstr.c_str(), "Log report - POST error", MB_OK | MB_ICONEXCLAMATION);
	}
	curl_global_cleanup();
	_endthread();
}

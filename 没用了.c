// params: remotefileaddr localfileaddr

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <curl/curl.h>

FILE* open_file(char* local_dir)
{
	FILE *file = fopen(local_dir, "w");
	if (file == NULL)
	{
		printf("Cannot open local file\n");
		exit(2);
	}
	printf("file open success\n");
	return file;
}

int main(int argc, char* argv[])
{
	if (argc < 3) return 1;
	FILE* file = open_file(argv[2]);

	CURL *curl;
	CURLcode curl_res;

	// init curl session
	curl = curl_easy_init();
	
	// set url
	curl_easy_setopt(curl, CURLOPT_URL, argv[1]);

	// set fd
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
	
	// start download
	curl_res = curl_easy_perform(curl);
	if (curl_res == 0)
	{
		printf("download success\n");
		long http_code;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		printf("http code: %ld\n", http_code);

		double size;
		curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD, &size);
		printf("size: %lf bytes\n", size);
	}
	else printf("ERROR\n");
	fclose(file);

	// cleanup
	curl_easy_cleanup(curl);
	
	return 0;
}

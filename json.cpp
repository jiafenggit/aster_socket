#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <iostream>

using namespace std;

struct json_keys {
	char *key;
	char *data;
};


int getJson(json_keys *keys, char *json, int start_pos, const char *parent) {
	char *new_json = (char*) calloc(0, sizeof(char*));
	char *sub_json_str = (char*) calloc(0, sizeof(char*));
	char *new_parent = (char*) calloc(0, sizeof(char*));
	int found_key = 1;
	int found_data = 0;
	int sub_json = 0;
	int i;
	int keys_count = start_pos;
	int data_count = start_pos;

	if (json[0] != '{' || json[strlen(json + 1)] != '}') {
		/* cout<<"Is not json"<<endl; */
		return 0;
	}

	for (i = 0; i < strlen(json); i++) {
		if (json[i] != ' ' && json[i] != '"' && json[i] != '\'') {
			new_json = (char*) realloc(new_json, (strlen(new_json) + strlen(&json[i])) * sizeof(char*));
			strncat(new_json, &json[i], 1);
		}
	}

	keys[keys_count].key = (char*) calloc(strlen(parent), sizeof(char*));
	strcpy(keys[keys_count].key, parent);

	for (i = 1; i < strlen(new_json) - 1; i++) {
		/* Начался json в json */
		if (new_json[i] == '{') {
			sub_json += 1;
			if (sub_json == 1) {
				sub_json_str = (char*) calloc(0, sizeof(char*));
			}
		}

		if (sub_json == 0) {

			/* Если ключ считывается то заносим данные к ключу, : означает конец ключа */
			if (found_key == 1 && new_json[i] != ':') {
				keys[keys_count].key = (char*) realloc(keys[keys_count].key, (strlen(keys[keys_count].key) + strlen(&new_json[i])) * sizeof(char*));
				strncat(keys[keys_count].key, &new_json[i],1);
			}

			/* Запятая означает начало считывания ключа */
			if (new_json[i] == ',') {
				keys[keys_count].key = (char*) calloc(strlen(parent), sizeof(char*));
				strcpy(keys[keys_count].key, parent);
				found_key = 1;
			}

			/* Если ключ считывается ищем :, она означает конец ключа */
			if (new_json[i] == ':' && found_key == 1) {
				keys_count++;
				found_key = 0;
			}

			/* Парсим значения ключей */

			/* Если данные считываются то заносим данные к данным, , означает конец данных */
			if (found_data == 1 && new_json[i] != ',') {
				keys[data_count].data = (char*) realloc(keys[data_count].data, (strlen(keys[data_count].data) + strlen(&new_json[i])) * sizeof(char*));
				strncat(keys[data_count].data, &new_json[i],1);
			}

			/* : означает начало считывания ключа */
			if (new_json[i] == ':') {
				keys[data_count].data = (char*) calloc(0, sizeof(char*));
				found_data = 1;
			}

			/* Если ключ считывается ищем , , она означает конец ключа */
			if (new_json[i] == ',' && found_data == 1) {
				data_count++;
				found_data = 0;
			}
		} else {
			keys[data_count].data = (char*) realloc(keys[data_count].data, (strlen(keys[data_count].data) + strlen(&new_json[i])) * sizeof(char*));
			strncat(keys[data_count].data, &new_json[i], 1);
		}

		/* Закончился json в json */
		if (new_json[i] == '}' && sub_json > 0) {
			sub_json -= 1;
			if (sub_json == 0) {
				new_parent = (char*) realloc(new_parent, (strlen(new_parent) + strlen(keys[keys_count - 1].key)) * sizeof(char*));
				
				strcat(new_parent, keys[keys_count - 1].key);
				strcat(new_parent, "_");
				keys_count = getJson(keys, keys[data_count].data, keys_count, new_parent);
				data_count = keys_count - 1;
			}
		}
	}

	return keys_count;
}

int main() {
	char json[] = "{\"test1\": \"{ddd: {ddd1: {ddd3: {ddd4:  {ddd5:  {ddd6: {ddd7: {ddd8: {ddd9: {ddd10: {ddd11: fff}}}}}}}, qwer:222}}}}\", 'test2': 2345, 'test2': 2345, 'test2': 2345, 'test2': 1111}";
	//char json[] = "{test1:123,test2:124}";
	json_keys *s = (json_keys*) calloc(100, sizeof(json_keys*));
	int i, c;

	c = getJson(s, json, 0, "");

	for (i = 0; i < c; i++) {

			cout<<i<<": "<<s[i].key<<"-"<<s[i].data<<endl;
		
	}


	return 0;
}

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
	int count;
	char *key;
	char *data;
};


int getJson(json_keys *keys, char *json, int start_pos) {
	char *new_json = (char*) calloc(1, sizeof(char*));
	char *sub_json_str = (char*) calloc(1, sizeof(char*));
	char *sub_json_str_n = (char*) calloc(1, sizeof(char*));
	char *tmp_key = (char*) calloc(1, sizeof(char*));
	int found_key = 0, sub_json = 0;
	int i = 0, j, keys_count;
	/* json_keys *keys = new json_keys; */
	keys_count = start_pos;

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

	for (i = strlen(new_json + 2); i >= 0; i--) {
		/* Начался json в json */
		if (new_json[i] == '}') {

			sub_json += 1;
		}

		/* парсим */
		if (sub_json == 0) {
			/* Если дошли до конца, то выводим последний ключ*/
			if (i == 0) {
				keys[keys_count].key = (char*) calloc(1, sizeof(char*));
				for (j = strlen(tmp_key) - 1; j >= 0; j--) {
					keys[keys_count].key = (char*) realloc(keys[keys_count].key, (strlen(keys[keys_count].key) + strlen(&tmp_key[j])) * sizeof(char*));
					strncat(keys[keys_count].key, &tmp_key[j],1);
				}
				keys_count++;
				found_key = 0;
			}

			/* Если ключ считывается то заносим данные к ключу, запятая означает конец ключа */
			if (found_key == 1 && new_json[i] != ',') {
				tmp_key = (char*) realloc(tmp_key, (strlen(tmp_key) + strlen(&new_json[i])) * sizeof(char*));
				strncat(tmp_key, &new_json[i], 1);
			}

			/* Двоеточие означает начало считывания ключа */
			if (new_json[i] == ':') {
				found_key = 1;
			}

			/* Если ключ считывается ищем запятую, она означает конец ключа */
			if (new_json[i] == ',' && found_key == 1) {
				keys[keys_count].key = (char*) calloc(1, sizeof(char*));
				for (j = strlen(tmp_key) - 1; j >= 0; j--) {
					keys[keys_count].key = (char*) realloc(keys[keys_count].key, (strlen(keys[keys_count].key) + strlen(&tmp_key[j])) * sizeof(char*));
					strncat(keys[keys_count].key, &tmp_key[j],1);
				}
				keys_count++;
				strcpy(tmp_key, "\0");
				tmp_key = (char*) realloc(tmp_key, 1 * sizeof(char*));
				found_key = 0;
			}
		} else {
			sub_json_str = (char*) realloc(sub_json_str, (strlen(sub_json_str) + strlen(&new_json[i])) * sizeof(char*));
			strncat(sub_json_str, &new_json[i], 1);
		}

		/* Закончился json в json */
		if (new_json[i] == '{' && sub_json > 0) {
			sub_json -= 1;
			if (sub_json == 0) {
				for (j = strlen(sub_json_str) - 1; j >= 0; j--) {
					sub_json_str_n = (char*) realloc(sub_json_str_n, (strlen(sub_json_str_n) + strlen(&sub_json_str[j])) * sizeof(char*));
					strncat(sub_json_str_n, &sub_json_str[j],1);
				}
				cout<<sub_json_str_n<<endl;
				
				
				keys_count += getJson(keys, sub_json_str_n, keys_count) - 1;
			}
		}
	}

	
	/* memcpy ( &main_key, &keys, sizeof(keys) ); */

	return keys_count;
}

int main() {
	char json[] = "{\"test1\": \"{ddd: {ddd1: {ddd3: {ddd4: fff}}} }\", 'test2': 2}";
	json_keys *s = new json_keys;
	int i, c;

	c = getJson(s, json, 0);
	cout<<c<<endl;

	for (i = 0; i < c; i++) {

			cout<<s[i].key<<endl;
		
	}

	return 0;
}

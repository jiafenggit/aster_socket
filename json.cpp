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

static char *m_strcpy(const char *src, const int count) {
	char *tmp;
	if (count == 0) {
		tmp = (char *) calloc(strlen(src), sizeof(char*));
		strcpy(tmp, src);
	} else {
		tmp = (char *) calloc(count, sizeof(char*));
		strncpy(tmp, src, count);
	}
	return tmp;
}

static char *m_strcat(char *dst, const char *src, const int count) {
	char *tmp;
	if (count == 0) {
		tmp = (char *) calloc(strlen(dst) + strlen(src), sizeof(char*));
		strcpy(tmp, dst);
		strcat(tmp, src);
	} else {
		tmp = (char *) calloc(strlen(dst) + count, sizeof(char*));
		strcpy(tmp, dst);
		strncat(tmp, src, count);
	}
	return tmp;
}

static int getJson(json_keys *keys, const char *json, const int start_pos, const char *parent) {
	char *new_json = (char*) calloc(0, sizeof(char*));
	char *new_parent = (char*) calloc(0, sizeof(char*));
	int found_key = 1;
	int found_data = 0;
	int sub_json = 0;
	int i;
	int keys_count = start_pos;
	int data_count = start_pos;

	if (json[0] != '{' || json[strlen(json + 1)] != '}')
		return 0;

	for (i = 0; i < strlen(json); i++)
		if (json[i] != ' ' && json[i] != '"' && json[i] != '\'')
			new_json = m_strcat(new_json, &json[i], 1);

	keys[keys_count].key = m_strcpy(parent, 0);

	for (i = 1; i < strlen(new_json) - 1; i++) {
		/* Начался json в json */
		if (new_json[i] == '{')
			sub_json += 1;

		if (sub_json == 0) {

			/* Если ключ считывается то заносим данные к ключу, : означает конец ключа */
			if (found_key == 1 && new_json[i] != ':') {
				keys[keys_count].key = m_strcat(keys[keys_count].key, &new_json[i], 1);
			}

			/* Запятая означает начало считывания ключа */
			if (new_json[i] == ',') {
				keys[keys_count].key = m_strcpy(parent, 0);
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
				keys[data_count].data = m_strcat(keys[data_count].data, &new_json[i], 1);
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
			keys[data_count].data = m_strcat(keys[data_count].data, &new_json[i], 1);
		}

		/* Закончился json в json */
		if (new_json[i] == '}' && sub_json > 0) {
			sub_json -= 1;
			if (sub_json == 0) {
				new_parent = m_strcat(new_parent, keys[keys_count - 1].key, 0);
				new_parent = m_strcat(new_parent, "_", 0);
				keys_count = getJson(keys, keys[data_count].data, keys_count, new_parent);
				data_count = keys_count - 1;
			}
		}
	}

	return keys_count;
}

int main() {
	char json[] = "{\"test1\": \"{ddd: {ddd1: {ddd3: {ddd4:  {ddd5:  {ddd6: {ddd7: {ddd8: {ddd9: {ddd10: {ddd11: fff}}}}}}}, qwer:{123:123}}}}}\", 'test2': 2345, 'test2': 2345, 'test2': 2345, 'test2': 1111}";
	//char json[] = "{1test1234567890qwertyuiopasdfg: ssddsdfdsssssss,2test21231234567900988565442345:123}";
	string qwe;
	char asd[] ="lol";
	json_keys *s = (json_keys*) calloc(100, sizeof(struct json_keys*));
	int i, c = 0;

	//s[0] = (json_keys) calloc(1, sizeof(struct json_keys));

	//s[0].key.append("sds");

	//*qwe = '\0';
	//cout<<sizeof(qwe)<<endl;
	//if (!qwe) {
	//	cout<<sizeof(qwe)<<endl;
	//	qwe = (char *) calloc(0, sizeof(char*));
	//}

	//m_strcpy(qwe, asd, 0,1);
	//qwe.append(json);
	//cout<<qwe<<endl;
	

	/*char *q;
	cout<<sizeof(q)<<endl;

	q = (char*) calloc(1, sizeof(char*));
	cout<<sizeof(q)<<endl;
	cout<<strlen(q)<<endl;

	q = (char*) realloc(q, (strlen(q) + 2222) * sizeof(char*));
	cout<<sizeof(q)<<endl;*/

	//s = (json_keys*) realloc(s, 23 * sizeof(json_keys*));

	//s[22].data = (char*) calloc(2, sizeof(char*));
	//s[22].data = (char*) realloc(s[22].data, (strlen(s[22].data) + strlen("LOL")) * sizeof(char*));
	//strncat(s[22].data, "LOL",3);
	//cout<<s[22].data<<endl;
	c = getJson(s, json, 0, "");


	for (i = 0; i < c; i++) {
			cout<<i<<": "<<s[i].key<<"-"<<s[i].data<<endl;
	}
	//cout<<s[0].key<<endl;


	return 0;
}

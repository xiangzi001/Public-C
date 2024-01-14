
size_t lengthString(string* str) {
	return sizeof(str) / sizeof(str[0]);
}

size_t posList(string str_1, string** str_2, size_t len) {
//	size_t len= lengthString(str_2);
	for (size_t i = 0; i < len; i++) {
		if (str_1 == (*(str_2[i]))  )
			return i;
	}
	return -1;
}
size_t posList(string str_1, char** str_2, size_t len) {
	//	size_t len= lengthString(str_2);
	for (size_t i = 0; i < len; i++) {
		if (str_1 == str_2[i])
			return i;
	}
	return -1;
}
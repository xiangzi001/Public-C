
int about(double n) {
	int m = int(n);
	int num = 0;
	if (n - m >= 0.5) {
		num = m + 1;
	}
	else{
		num = m;
	}
	return num;
}



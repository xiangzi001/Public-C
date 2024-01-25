
template <typename T>
double Norm_one(MAT<T> A) {
	T ret = 0;
	for (int j = 0; j < A.cols; j++) {
		T temp = 0;
		for (int i = 0; i < A.rows; i++) {
			temp += fabs(A[i][j]);
		}
		if (temp > ret)
			ret = temp;
	}
	A.data = 0;
	return ret;
}

template <typename T>
double Norm_inf(MAT<T> A) {
	T ret = 0;
	for (int i = 0; i < A.rows; i++) {
		T temp = 0;
		for (int j = 0; j < A.cols; j++) {
			temp += fabs(A[i][j]);
		}
		if (temp > ret)
			ret = temp;
	}
	A.data = 0;
	return ret;
}

template <typename T>
double Norm_F(MAT<T> A) {
	T ret = 0;
	for (int i = 0; i < A.rows*A.cols; i++) {
		ret += A.data[i] * A.data[i];
	}
	A.data = 0;
	return sqrt(ret);
}

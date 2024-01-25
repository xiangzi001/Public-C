
template <typename T>
void random(MAT<T>& A, int r, int c, int Big, int Small) {
	A.rows = r;
	A.cols = c;
	int s = r * c;
	delete[] A.data;
	A.data = new T[s];
	int temp = r;
	for (int i = 0; i < s; i++) {
		random(temp,i);
		temp = fabs(temp);
		A.data[i] = (temp % (Big - Small)) + Small;
	}
	//for (int i = 0; i < s; i++) {
	//	srand((int)(time(0)) + i);
	//	A.data[i] = (rand() % (Big - Small)) + Small;
	//}
}

template<typename t>
MAT<t> T(MAT<t> A) {
	MAT<t> n(A.cols, A.rows);
	for (int i = 0; i < A.cols; i++)
		for (int j = 0; j < A.rows; j++) {
			n[i][j] = A[j][i];
		}
	A.data = 0;
	return n;
}
template<typename t>
MAT<t> trans(MAT<t> A) {
	MAT<t> n(A.cols, A.rows);
	for (int i = 0; i < A.cols; i++)
		for (int j = 0; j < A.rows; j++) {
			n[i][j] = A[j][i];
		}
	A.data = 0;
	return n;
}

template<typename t>
MAT<t> copy(MAT<t>& A) {
	MAT<t> n(A.cols, A.rows);
	for (int i = 0; i < A.cols * A.rows; i++)
		n.data[i] = A.data[i];
	return n;
}

template<typename T>
void times(MAT<T>& A, MAT<T>& B, MAT<T>& C) {
	int pos = C.rows * C.cols;
	for (int i = 0; i < pos; i++) {
		C.data[i] = 0;
	}
	T v;
	for (int i = 0; i < C.rows; ++i) {
		for (int k = 0; k < A.cols; ++k) {
			v = A[i][k];
			for (int j = 0; j < C.cols; ++j)
				C[i][j] += v * B[k][j];
		}
	}
}

template<typename T>
void times_A_BT(MAT<T>& A, MAT<T>& B, MAT<T>& C) {
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < B.rows; j++) {
			C[i][j] = 0;
			for (int k = 0; k < A.cols; k++)
				C[i][j] = C[i][j] + A[i][k] * B[j][k];
		}
	}
}

template<typename T>
void times_AT_B(MAT<T>& A, MAT<T>& B, MAT<T>& C) {
	int size = C.rows * C.cols;
	for (int i = 0; i < size; i++)
		C.data[i] = 0;
	for (int k = 0; k < A.rows; k++) {
		for (int i = 0; i < C.rows; i++) {
			for (int j = 0; j < C.cols; j++) {
				C[i][j] = C[i][j] + A[k][i] * B[k][j];
			}
		}
	}
}

template<typename T>
int Rank(MAT<T>& A, T eps=0){
	int r, c;
	for (r = 0, c = 0; c < A.cols; c++) {

		// 1. �ҵ����ֵ
		int t = r;
		for (int i = r; i < A.cols; i++) {
			if (fabs(A[i][c]) > fabs(A[t][c]))
				t = i;
		}

		if (fabs(A[t][c]) < eps) continue;

		// 2. ����������
		for (int i = c; i < A.rows; i++) swap(A[r][i], A[t][i]);

		// 3. ����������Ϊ1
		for (int i = A.rows - 1; i >= c; i--) A[r][i] /= A[r][c];

		// 4. �������������Ϊ0
		for (int i = r + 1; i < A.cols; i++) {
			if (fabs(A[i][c]) < eps) continue;
			for (int j = A.rows - 1; j >= c; j--) {
				A[i][j] -= A[i][c] * A[r][j]; // - �׺����� * i, r������
			}
		}

		r++;
	}
	return r;
}

template<typename T>
int Rank_c(MAT<T>& A, T eps = 0) {
	MAT<T> B(copy(A));
	return Rank(B);
}



template<typename T>
T det(MAT<T> n) {
	double r = 1;
	for (int y = 0; y < n.cols; y++) {
		if (n[y][y] == 0)
			for (int k = y + 1; k < n.rows; k++)
				if (n[k][y]) {
					T temp;
					for (int i = 0; i < n.cols; i++) {
						temp = n[k][i];
						n[k][i] = n[y][i];
						n[y][i] = temp;
					}
					r = -r;
					break;
				}
				else {
					return 0;
				}
		for (int x = 0; x < n.rows; x++) {
			if (x > y) {
				double temp(-n[x][y] / n[y][y]);
				for (int k = y; k < n.rows; k++)
					n[x][k] = n[x][k] + n[y][k] * temp;
			}
		}
	}
	for (int x = 0; x < n.rows; x++)
		r = r * n[x][x];
	n.data = 0;
	return r;
}

template<typename T>
T det_c(MAT<T> n) {
	MAT<T> m(n.rows, n.cols, n.data);
	n.data = 0;
	return det(m);
}

template<typename T>
void solve(MAT<T>& A, MAT<T>& b) {
	for (int y = 0; y < A.cols; y++) {
		if (A[y][y] == 0)
			for (int k = y + 1; k < A.rows; k++)
				if (A[k][y] != 0) {
					T temp;
					for (int i = y; i < A.cols; i++) {
						temp = A[k][i];
						A[k][i] = A[y][i];
						A[y][i] = temp;
					}
					for (int i = y; i < b.cols; i++) {
						temp = b[k][i];
						b[k][i] = b[y][i];
						b[y][i] = temp;
					}
					break;
				}
		for (int x = y + 1; x < A.rows; x++) {
			T temp(-A[x][y] / A[y][y]);
			for (int k = y; k < A.rows; k++)
				A[x][k] = A[x][k] + A[y][k] * temp;
			for (int k = 0; k < b.cols; k++)
				b[x][k] = b[x][k] + b[y][k] * temp;
		}
	}
	/*
	for (int i = 0; i < A.rows; i++)
		if (A[i][i] != 0) {
			T temp = 1 / A[i][i];
			for (int n = i; n < A.cols; n++)
				A[i][n] = A[i][n] * temp;
			for (int n = 0; n < b.cols; n++)
				b[i][n] = b[i][n] * temp;
		}
		else return;

	for (int y = A.cols - 1; y > 0; y--)
		for (int x = 0; x < y; x++) {
			for (int n = 0; n < b.cols; n++)
				b[x][n] = b[x][n] - A[x][y] * b[y][n];
		}
	*/
	for (int y = A.cols - 1; y >= 0; y--) {
		T temp_1 = 1 / A[y][y];
		for (int x = 0; x < y; x++) {
			for (int n = 0; n < b.cols; n++) {
				b[x][n] = b[x][n] - b[y][n] * A[x][y] * temp_1;
			}
		}
		for (int n = 0; n < b.cols; n++) {
			b[y][n] = b[y][n] * temp_1;
		}
	}
}

template<typename T>
MAT<T> solve_c(MAT<T>& A, MAT<T>& b) {
	MAT<T> B(A.rows, A.cols, A.data);
	MAT<T> c(b.rows, b.cols, b.data);
	solve(B, c);
	return c;
}

template<typename T>
void solve_0(MAT<T>& A, T* b, int n) {

	for (int j = 0; j < n; j++) {
		T temp = 1 / A[j][j];
		for (int k = j; k < A.cols; k++)
			A[j][k] = A[j][k] * temp;
	}

	for (int j = n - 1; j > 0; j--) {
		for (int x = 0; x < j; x++) {
			A[x][n] = A[x][n] - A[x][j] * A[j][n];
		}
	}

	T len = 1;
	for (int j = 0; j < n; j++) {
		len = len + A[j][n] * A[j][n];
	}
	len = 1.0 / sqrt(len);

	for (int j = 0; j < n; j++) {
		b[j] = A[j][n] * len;
	}
	b[n] = -len;
}

template<typename T>
void solve_0(MAT<T>& A, T* b) {
	//int i = 0;
	int n = A.cols;

	for (int y = 0; y < A.cols; y++) {
		if (A[y][y] == 0) {
			n = y;
			break;
		}
		for (int x = y + 1; x < A.rows; x++) {
			T temp(-A[x][y] / A[y][y]);
			for (int k = y; k < A.rows; k++)
				A[x][k] = A[x][k] + A[y][k] * temp;
		}
	}

	solve_0(A, b, n);
}

template<typename T>
MAT<T> Inv(MAT<T>& A) {
	MAT<T> I;
	I.iden(A.rows);
	solve(A, I);
	return I;
}

template<typename T>
MAT<T> Inv_c(MAT<T>& A) {
	MAT<T> I;
	I.iden(A.rows);
	MAT<T> B(A.rows, A.cols, A.data);
	solve(B, I);
	return I;
}


enum SVD_type {
	_SVD_QR,
	_EVD_QR,
	_EVD_Jacobi
};

template<typename T>
void HouseHolder_SVD_left(MAT<T>& A, MAT<T>& U, T* &ur, int r, int len) {
	int i, j, k;
	T temp, sum, cr, hr;
	temp = 0;
	for (k = r; k < A.rows; k++)
		temp += fabs(A[k][r]);
	if (temp > 0)
	{
		sum = 0;
		for (k = len; k < A.rows; k++)
			sum += A[k][r] * A[k][r];
		cr = sqrt(sum);
		if (A[len][r] > 0.0)cr = -cr;
		hr = cr * (cr - A[len][r]);

		//定义ur
		ur[len] = A[len][r] - cr;
		for (i = len + 1; i < A.rows; i++)
		{
			ur[i] = A[i][r];
		}

		//定义A
		for (j = r; j < A.cols; j++)
		{
			sum = 0;
			for (i = len; i < A.rows; i++)
				sum += A[i][j] * ur[i];
			sum = sum / hr;
			for (i = len; i < A.rows; i++)
				A[i][j] = A[i][j] - ur[i] * sum;
		}

		//定义U
		for (i = 0; i < U.rows; i++)
		{
			sum = 0;
			for (j = len; j < U.rows; j++)
				sum += U[i][j] * ur[j];
			sum = sum / hr;
			for (j = len; j < U.cols; j++)
			{
				U[i][j] = U[i][j] - ur[j] * sum;
			}
		}
	}
}
template<typename T>
void HouseHolder_SVD_right(MAT<T>& A, MAT<T>& V, T*& ur, int r, int len) {
	int i, j, k;
	T temp, sum, cr, hr;
	temp = 0;
	for (k = r; k < A.cols; k++)
		temp += fabs(A[r][k]);
	if (temp > 0)
	{
		sum = 0;
		for (k = len; k < A.cols; k++)
			sum += A[r][k] * A[r][k];
		cr = sqrt(sum);
		if (A[r][len] > 0.0)cr = -cr;
		hr = cr * (cr - A[r][len]);

		//定义ur
		ur[len] = A[r][len] - cr;
		for (i = len+1; i < A.cols; i++)
		{
			ur[i] = A[r][i];
		}
		
		//定义A
		for (j = r; j < A.rows; j++)
		{
			sum = 0;
			for (i = len; i < A.cols; i++)
				sum += A[j][i] * ur[i];
			sum = sum / hr;
			for (i = len; i < A.cols; i++)
				A[j][i] = A[j][i] - ur[i] * sum;
		}
		//定义V
		for (i = 0; i < V.rows; i++)
		{
			sum = 0;
			for (j = len; j < V.cols; j++)
				sum += V[i][j] * ur[j];
			sum = sum / hr;
			for (j = len; j < V.cols; j++)
			{
				V[i][j] = V[i][j] - ur[j] * sum;
			}
		}

	}
}

template<typename T>
void Givens_SVD_left(MAT<T>& A, MAT<T>& U, int& i, int& j, int& k, double& theta, T& c, T& s, T& temp) {
	theta = atan2(A[i][j], A[i - 1][j]);
	c = cos(theta);
	s = sin(theta);
	for (k = j; k < A.cols; k++) {
		temp = c * A[i - 1][k] + s * A[i][k];
		A[i][k] = -s * A[i - 1][k] + c * A[i][k];
		A[i - 1][k] = temp;
	}
	for (k = 0; k < U.rows; k++) {
		temp = c * U[k][i - 1] + s * U[k][i];
		U[k][i] = -s * U[k][i - 1] + c * U[k][i];
		U[k][i - 1] = temp;
	}
}

template<typename T>
void Givens_SVD_right(MAT<T>& A, MAT<T>& V, int& i, int& j, int &k, double& theta, T& c, T& s, T& temp) {
	theta = atan2(A[j][i], A[j][i - 1]);
	c = cos(theta);
	s = sin(theta);
	for (k = j; k < A.rows; k++) {
		temp = c * A[k][i - 1] + s * A[k][i];
		A[k][i] = -s * A[k][i - 1] + c * A[k][i];
		A[k][i - 1] = temp;
	}
	for (k = 0; k < V.rows; k++) {
		temp = c * V[k][i - 1] + s * V[k][i];
		V[k][i] = -s * V[k][i - 1] + c * V[k][i];
		V[k][i - 1] = temp;
	}
}

template<typename T>
void Givens_SVD_UP(MAT<T>& A, MAT<T>& U, MAT<T>& V, int m, int n, T& eps) {
	int x,y,k;
	double theta; 
	T c,s,temp;
	while (n - m - 1) {

		if (fabs(A[m][m + 1]) < eps) {
			m++;
			continue;
		}
		x = m + 1;
		y = m;

		Givens_SVD_right(A, V, x, y, k, theta, c, s, temp);
		Givens_SVD_left(A, U, x, y, k, theta, c, s, temp);

		for (x = m + 2; x < n; x++) {
			if (fabs(A[x - 1][x]) < eps) {
				if (n - x - 1) {
					Givens_SVD_UP(A, U, V, m, x, eps);
					m = x;
					break;
				}
				else {
					n--;
					break;
				}
			}
			y = x - 2;
			Givens_SVD_right(A, V, x, y, k, theta, c, s, temp);
			y++;
			Givens_SVD_left(A, U, x, y, k, theta, c, s, temp);
		}
	}
}

template<typename T>
void Givens_SVD_DOWN(MAT<T>& A, MAT<T>& U, MAT<T>& V, int m, int n, T& eps) {
	int x, y, k;
	double theta;
	T c, s, temp;
	while (n - m - 1) {

		if (fabs(A[m+1][m]) < eps) {
			m++;
			continue;
		}
		//print(A);
		x = m + 1;
		y = m;

		Givens_SVD_left(A, U, x, y, k, theta, c, s, temp);
		Givens_SVD_right(A, V, x, y, k, theta, c, s, temp);

		for (x = m + 2; x < n; x++) {
			if (fabs(A[x][x - 1]) < eps) {
				if (n - x - 1) {
					Givens_SVD_DOWN(A, U, V, m, x, eps);
					m = x;
					break;
				}
				else {
					n--;
					break;
				}
			}
			y = x - 2;
			Givens_SVD_left(A, U, x, y, k, theta, c, s, temp);
			y++;
			Givens_SVD_right(A, V, x, y, k, theta, c, s, temp);
		}
	}
}

template<typename T>
void SVD_QR(MAT<T>& A, MAT<T>& U, MAT<T>& E, MAT<T>& V, T eps) {
	int i = 0;
	int s = U.rows * U.cols;
	for (i = 0; i < s; i++) {
		U.data[i] = 0;
	}
	for (i = 0; i < U.rows; i++) {
		U[i][i] = 1;
	}
	s = V.rows * V.cols;
	for (i = 0; i < s; i++) {
		V.data[i] = 0;
	}
	for (i = 0; i < V.rows; i++) {
		V[i][i] = 1;
	}
	s = A.rows * A.cols;
	for (i = 0; i < s; i++) {
		E.data[i] = A.data[i];
	}

	if (E.rows > E.cols) {
		T* ur = new T[E.rows];
		s = E.cols - 1;
		for (int r = 0; r < s; r++) {
			HouseHolder_SVD_left(E, U, ur, r, r);
			HouseHolder_SVD_right(E, V, ur, r, r+1);
		}
		HouseHolder_SVD_left(E, U, ur, s, s);

		Givens_SVD_UP(E, U, V, 0, E.cols, eps);

		delete[] ur;
	}
	else {
		T* ur = new T[E.cols];
		int r;
		s = E.rows - 1;
		for (r = 0; r < s; r++) {
			HouseHolder_SVD_right(E, V, ur, r, r);
			HouseHolder_SVD_left(E, U, ur, r, r+1);
		}
		HouseHolder_SVD_right(E, V, ur, s, s);

		Givens_SVD_DOWN(E, U, V, 0, E.rows, eps);

		delete[] ur;
	}
}

template<typename T>
void SVD_QR_old1(MAT<T>& A, MAT<T>& U, MAT<T>& E, MAT<T>& V, T eps) {
	int i = 0;
	int s = U.rows * U.cols;
	for (i = 0; i < s; i++) {
		U.data[i] = 0;
	}
	for (i = 0; i < U.rows; i++) {
		U[i][i] = 1;
	}
	s = V.rows * V.cols;
	for (i = 0; i < s; i++) {
		V.data[i] = 0;
	}
	for (i = 0; i < V.rows; i++) {
		V[i][i] = 1;
	}
	s = A.rows * A.cols;
	for (i = 0; i < s; i++) {
		E.data[i] = A.data[i];
	}

	if (E.rows > E.cols) {
		T* ur = new T[E.rows];
		for (int r = 0; r < E.cols; r++) {
			//HouseHolder_SVD_left(E, U, ur, r, r);
			//HouseHolder_SVD_right(E, V, ur, r, r+1);
		}
		int m = 0;
		int n = E.cols;
		while (n - m) {
			Givens_SVD_UP(E, U, V, m, n);

			for (; m < n; m++) {
				if (fabs(E[m][m + 1]) > eps) {
					break;
				}
			}
			for (; n > m; n--) {
				if (fabs(E[n - 2][n - 1]) > eps) {
					break;
				}
			}
		}
		delete[] ur;
	}
	else {
	}
}

template <typename T>
void SVD_EVD_QR(MAT<T>& A, MAT<T>& U, MAT<T>& E, MAT<T>& V, T eps) {
	if (A.rows > A.cols) {
		//T(A)*A = V*S*T(V)
		MAT<T> B(A.cols, A.cols);

		times_AT_B(A, A, B);

		EVD_Q_QR(B, V, eps);

		for (int i = 0; i < A.cols; i++) {
			E[i][i] = sqrt(B[i][i]);
		}

		//U*E = A*V
		for (int i = 0; i < A.rows; i++) {
			for (int j = 0; j < A.cols; j++) {
				U[i][j] = 0;
				for (int k = 0; k < A.cols; k++) {
					U[i][j] = U[i][j] + A[i][k] * V[k][j];
				}
				U[i][j] = U[i][j] / E[j][j];
			}
		}
	}
	else {
		//A*T(A) = U*S*T(U)
		MAT<T> B(A.cols, A.rows);
		B.rows = A.rows;

		times_A_BT(A, A, B);

		EVD_Q_QR(B, U, eps);

		for (int i = 0; i < A.rows; i++) {
			E[i][i] = sqrt(B[i][i]);
		}

		//V*E = T(A)*U
		B.rows = A.cols;
		times_AT_B(A, U, B);

		for (int i = 0; i < A.cols; i++) {
			for (int j = 0; j < A.rows; j++) {
				V[i][j] = B[i][j] / E[j][j];
			}
		}
	}

}

template <typename T>
void SVD_EVD_Jacobi(MAT<T>& A, MAT<T>& U, MAT<T>& E, MAT<T>& V, T eps) {
	if (A.rows > A.cols) {
		//T(A)*A = V*S*T(V)
		MAT<T> B(A.cols, A.cols);

		times_AT_B(A, A, B);

		EVD_Q_Jacobi(B, V, eps);

		for (int i = 0; i < A.cols; i++) {
			E[i][i] = sqrt(B[i][i]);
		}

		//U*E = A*V
		B.rows = A.rows;

		for (int i = 0; i < A.rows; i++) {
			for (int j = 0; j < A.cols; j++) {
				U[i][j] = 0;
				for (int k = 0; k < A.cols; k++) {
					U[i][j] = U[i][j] + A[i][k] * V[k][j];
				}
				U[i][j] = U[i][j] / E[j][j];
			}
		}
	}
	else {
		//A*T(A) = U*S*T(U)
		MAT<T> B(A.cols, A.rows);
		B.rows = A.rows;

		times_A_BT(A, A, B);

		EVD_Q_Jacobi(B, U, eps);

		for (int i = 0; i < A.rows; i++) {
			E[i][i] = sqrt(B[i][i]);
		}

		//V*E = T(A)*U
		B.rows = A.cols;
		times_AT_B(A, U, B);

		for (int i = 0; i < A.cols; i++) {
			for (int j = 0; j < A.rows; j++) {
				V[i][j] = B[i][j] / E[j][j];
			}
		}
	}
}

// A(r, c) U(r, r) E(r, c) V(c, c)
template<typename T>
void SVD(MAT<T>& A, MAT<T>& U, MAT<T>& E, MAT<T>& V, T eps, SVD_type type) {
	switch (type) {
	case _SVD_QR: {
		SVD_QR(A, U, E, V, eps);
		break;
	}
	case _EVD_QR: {
		SVD_EVD_QR(A, U, E, V, eps);
		break;
	}
	case _EVD_Jacobi: {
		SVD_EVD_Jacobi(A, U, E, V, eps);
		break;
	}
	default:
		break;
	}
}

void bidiagonalize(double** matrix, int m, int n, double** U, double** V) {
	// 双对角化过程
	for (int k = 0; k < n; k++) {
		double x = 0.0;
		for (int i = k; i < m; i++) {
			x += matrix[i][k] * matrix[i][k];
		}
		double sigma = sqrt(x);
		if (matrix[k][k] > 0) {
			sigma = -sigma;
		}
		double alpha = sqrt(2 * x - 2 * matrix[k][k] * sigma);
		double beta = 1 / (sqrt(2 * x));
		matrix[k][k] -= sigma;
		for (int i = k + 1; i < m; i++) {
			matrix[i][k] = beta * matrix[i][k];
		}
		for (int j = k + 1; j < n; j++) {
			double sum = 0.0;
			for (int i = k; i < m; i++) {
				sum += matrix[i][k] * matrix[i][j];
			}
			for (int i = k; i < m; i++) {
				matrix[i][j] -= 2 * matrix[i][k] * sum;
			}
		}
		if (k < n - 2) {
			x = 0.0;
			for (int j = k + 1; j < n; j++) {
				x += matrix[k][j] * matrix[k][j];
			}
			sigma = sqrt(x);
			if (matrix[k][k + 1] > 0) {
				sigma = -sigma;
			}
			alpha = sqrt(2 * x - 2 * matrix[k][k + 1] * sigma);
			beta = 1 / (sqrt(2 * x));
			matrix[k][k + 1] -= sigma;
			for (int j = k + 2; j < n; j++) {
				matrix[k][j] = beta * matrix[k][j];
			}
			for (int i = k; i < m; i++) {
				double sum = 0.0;
				for (int j = k + 1; j < n; j++) {
					sum += matrix[i][j] * matrix[k][j];
				}
				for (int j = k + 1; j < n; j++) {
					matrix[i][j] -= 2 * matrix[k][j] * sum;
				}
			}
		}
	}

	// 构造 U 和 V 矩阵
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < m; j++) {
			if (i == j) {
				U[i][j] = 1.0;
			}
			else {
				U[i][j] = 0.0;
			}
		}
	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (i == j) {
				V[i][j] = 1.0;
			}
			else {
				V[i][j] = 0.0;
			}
		}
	}
}
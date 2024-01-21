
enum QR_type {
    _HouseHolder,
    _Givens
};

template<typename T>
void QR_HouseHolder(MAT<T>& Q, MAT<T>& R, T*ur) {
    int n = 0;
    if (R.rows > R.cols) {
        n = R.cols;
    }
    else {
        n = R.rows;
    };
    int i, j, k, r;

    T temp, sum, cr, hr;

    //定义单位矩阵
    int s = n * n;
    for (int i = 0; i < s; i++)
        Q.data[i] = 0;
    for (int i = 0; i < n; i++)
        Q.data[i * (n + 1)] = 1;

    for (r = 0; r < n; r++) {
        temp = 0;
        for (k = r; k < n; k++)
            temp += fabs(R[k][r]);

        if (temp > 0)
        {
            sum = 0;

            for (k = r; k < n; k++)
                sum += R[k][r] * R[k][r];

            cr = sqrt(sum);

            if (R[r][r] > 0.0)cr = -cr;

            hr = cr * (cr - R[r][r]);

            for (i = 0; i < n; i++)//定义ur
            {
                if (i < r)ur[i] = 0;
                if (i == r)ur[i] = R[r][r] - cr;
                if (i > r)ur[i] = R[i][r];
            }
            for (i = 0; i < R.rows; i++)//定义Q
            {
                sum = 0;
                for (j = 0; j < n; j++)
                    sum += Q[i][j] * ur[j];
                sum = sum / hr;
                for (j = 0; j < R.cols; j++)
                {
                    Q[i][j] = Q[i][j] - ur[j] * sum;
                }
            }
            for (j = 0; j < R.cols; j++)//定义R
            {
                sum = 0;
                for (i = 0; i < n; i++)
                    sum += R[i][j] * ur[i];
                sum = sum / hr;
                for (i = 0; i < R.rows; i++)
                    R[i][j] = R[i][j] - ur[i] * sum;
            }

        }
    }
}

template<typename T>
void QR_Givens(MAT<T>& Q, MAT<T>& R,T eps=0) {
    for (int i = 0; i < R.rows; i++) {
        for (int j = 0; j < R.rows; j++) {
            if (i == j) {
                Q[i][j] = 1.0;
            }
            else {
                Q[i][j] = 0.0;
            }
        }
    }

    for (int j = 0; j < R.cols; j++) {
        for (int i = R.rows - 1; i > j; i--) {
            if (fabs(R[i][j]) > eps) {
                T theta = atan2(R[i][j], R[i - 1][j]);
                T c = cos(theta);
                T s = sin(theta);

                for (int k = 0; k < R.cols; k++) {
                    T temp = c * R[i - 1][k] + s * R[i][k];
                    R[i][k] = -s * R[i - 1][k] + c * R[i][k];
                    R[i - 1][k] = temp;
                }

                for (int k = 0; k < R.rows; k++) {
                    T temp = c * Q[k][i - 1] + s * Q[k][i];
                    Q[k][i] = -s * Q[k][i - 1] + c * Q[k][i];
                    Q[k][i - 1] = temp;
                }
            }
        }
    }
}

template<typename T>
void QR(MAT<T>& Q, MAT<T>& R, QR_type type) {
    switch (type)
    {
    case _HouseHolder: {
        T* ur = new T[R.cols];
        QR_HouseHolder(Q, R,ur);
        delete[] ur;
        break;
    }
    case _Givens: {
        QR_Givens(Q, R);
        break;
    }
    default:
        break;
    }
}



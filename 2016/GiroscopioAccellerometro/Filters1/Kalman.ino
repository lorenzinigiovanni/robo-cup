// KasBot V1  -  Kalman filter module

    float Q_angle  =  0.01; //0.001    //0.005
    float Q_gyro   =  0.0003;  //0.003  //0.0003
    float R_angle  =  0.01;  //0.03     //0.008

    float x_bias = 0;
    float P_00 = 0, P_01 = 0, P_10 = 0, P_11 = 0;	
    float  y, S;
    float K_0, K_1;

  float kalmanCalculate(float newAngle, float newRate,int looptime)
  {
    float dt = float(looptime)/1000;                                    // XXXXXXX arevoir
    x_angle += dt * (newRate - x_bias);
    P_00 +=  - dt * (P_10 + P_01) + Q_angle * dt;
    P_01 +=  - dt * P_11;
    P_10 +=  - dt * P_11;
    P_11 +=  + Q_gyro * dt;
    
    y = newAngle - x_angle;
    S = P_00 + R_angle;
    K_0 = P_00 / S;
    K_1 = P_10 / S;
    
    x_angle +=  K_0 * y;
    x_bias  +=  K_1 * y;
    P_00 -= K_0 * P_00;
    P_01 -= K_0 * P_01;
    P_10 -= K_1 * P_00;
    P_11 -= K_1 * P_01;
    
    return x_angle;
  }


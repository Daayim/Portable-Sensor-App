#include <QCoreApplication>
#include <math.h>
#include <inttypes.h>

/*
int main(int argc, char **argv)
{
    QCoreApplication a(argc, argv);

    // Calibration data stored in PROM
    uint16_t c1;
    uint16_t c2;
    uint16_t c3;
    uint16_t c4;
    uint16_t c5;
    uint16_t c6;

    // Digital pressure and temperature
    uint32_t d1;
    uint32_t d2;

    // Temperature
    int32_t dt;
    int32_t temp;

    // Temperature compensated pressure
    int64_t off;
    int64_t sens;
    int32_t p;

    // Second order temperature compensation
    int64_t t2;
    int64_t off2;
    int64_t sens2;

    c1 = 46372;
    c2 = 43981;
    c3 = 29059;
    c4 = 27842;
    c5 = 31553;
    c6 = 28165;

    d1 = 6465444;
    d2 = 8077636;

    c1 = 40397;
    c2 = 42219;
    c3 = 23839;
    c4 = 24724;
    c5 = 30442;
    c6 = 26383;

    d1 = 767063;
    d2 = 977573;

    d1 = 851215;
    d2 = 1129397;

    // Calculate temperature
    // dT = D2 - TREF = D2 - C5 * 2^8
    // TEMP = 20°C + dT * TEMPSENS = 2000 + dT * C6 / 2^23
    dt = (int32_t)d2 - ((int32_t)c5 << 8);
    temp = 2000 + ((int64_t)dt * (int64_t)c6 >> 23);


    double t;
    t = (double)d2 - (double)c5 * pow(2.0, 8);
    t *= (double)c6 / pow(2.0, 23);
    t += 2000;
    printf("t = %.2lf\n\n", t);


    // Calculate temperature compensated pressure
    // OFF = OFFT1 + TCO * dT = C2 * 2^17 + (C4 * dT ) / 2^6
    // SENS = SENST1 + TCS * dT = C1 * 2^16 + (C3 * dT ) / 2^7
    // P = D1 * SENS - OFF = (D1 * SENS / 2^21 - OFF) / 2^15
    off = ((int64_t)c2 << 17) + (((int64_t)c4 * dt) >> 6);
    sens = ((int64_t)c1 << 16) + (((int64_t)c3 * dt) >> 7);
    p = (((d1 * sens) >> 21) - off) >> 15;

    printf("Temperature:\n");
    printf("dt = %d\n", dt);
    printf("temp = %.2lf\n\n", (double)temp / 100.0);

    printf("Temperature compensated pressure:\n");
    printf("off = %lld\n", off);
    printf("sens = %lld\n", sens);
    printf("p = %.2lf\n\n", (double)p / 100.0);


    t = (double)dt * (double)dt;
    t *= 3.0;
    t /= pow(2.0, 33);
    t = temp - t;
    printf("t = %.2lf\n\n", t);


    // Second order temperature compensation
    if (temp < 2000)
    {
        // Low temperature
        // T2 = 3 * dT^2 / 2^33
        // OFF2 = 61 * (TEMP - 2000)^2 / 2^4
        // SENS2 = 29 * (TEMP - 2000)^2 / 2^4
        t2 = (3 * ((int64_t)dt * (int64_t)dt)) >> 33;
        off2 = 61 * ((int64_t)temp - 2000) * ((int64_t)temp - 2000) / 16;
        sens2 = 29 * ((int64_t)temp - 2000) * ((int64_t)temp - 2000) / 16;

        if (temp < -1500)
        {
            // Very low temperature
            // OFF2 = OFF2 + 17 * (TEMP + 1500)^2
            // SENS2 = SENS2 + 9 * (TEMP + 1500)^2
            off2 += 17 * ((int64_t)temp + 1500) * ((int64_t)temp + 1500);
            sens2 += 9 * ((int64_t)temp + 1500) * ((int64_t)temp + 1500);
        }
    }
    else
    {
        // High temperature
        // T2 = 5 * dT^2 / 2^38
        // OFF2 = 0
        // SENS2 = 0
        t2 = (5 * ((int64_t)dt * (int64_t)dt)) >> 38;
        off2 = 0;
        sens2 = 0;
    }

    temp -= t2;
    off -= off2;
    sens -= sens2;

    p = (((d1 * sens) >> 21) - off) >> 15;

    printf("Second order temperature compensation:\n");
    printf("t2 = %lld\n", t2);
    printf("off2 = %lld\n", off2);
    printf("sens2 = %lld\n", sens2);
    printf("off = %lld\n", off);
    printf("sens = %lld\n", sens);
    printf("temp = %.2lf\n", (double)temp / 100.0);
    printf("p = %.2lf\n", (double)p / 100.0);

    return a.exec();
}
*/


int main(int argc, char **argv)
{
    QCoreApplication a(argc, argv);

    // Calibration data stored in PROM
    uint16_t c1;
    uint16_t c2;
    uint16_t c3;
    uint16_t c4;
    uint16_t c5;
    uint16_t c6;

    // Digital pressure and temperature
    uint32_t d1;
    uint32_t d2;

    // Temperature
    double dt;
    double temp;

    // Temperature compensated pressure
    double off;
    double sens;
    double p;

    // Second order temperature compensation
    double t2;
    double off2;
    double sens2;

    c1 = 46372;
    c2 = 43981;
    c3 = 29059;
    c4 = 27842;
    c5 = 31553;
    c6 = 28165;

    d1 = 6465444;
    d2 = 8077636;

    c1 = 40397;
    c2 = 42219;
    c3 = 23839;
    c4 = 24724;
    c5 = 30442;
    c6 = 26383;

    d1 = 767063;
    d2 = 977573;

    d1 = 851215;
    d2 = 1129397;

    d1 = 790099;
    d2 = 1076985;

    // Calculate temperature
    // dT = D2 - TREF = D2 - C5 * 2^8
    // TEMP = 20°C + dT * TEMPSENS = 2000 + dT * C6 / 2^23
    dt = (double)d2 - (double)c5 * pow(2, 8);
    temp = 2000.0 + dt * (double)c6 / pow(2, 23);

    // Calculate temperature compensated pressure
    // OFF = OFFT1 + TCO * dT = C2 * 2^17 + (C4 * dT ) / 2^6
    // SENS = SENST1 + TCS * dT = C1 * 2^16 + (C3 * dT ) / 2^7
    // P = D1 * SENS - OFF = (D1 * SENS / 2^21 - OFF) / 2^15
    off = (double)c2 * pow(2, 17) + (double)c4 * dt / pow(2, 6);
    sens = (double)c1 * pow(2, 16) + (double)c3 * dt / pow(2, 7);
    p = ((double)d1 * sens / pow(2, 21) - off) / pow(2, 15);

    printf("Temperature:\n");
    printf("dt = %.2lf (0x%x)\n", dt, (int32_t)dt);
    printf("temp = %.2lf (0x%x)\n\n", temp / 100.0, (int32_t)(temp / 100.0));

    printf("Temperature compensated pressure:\n");
    printf("off = %.2lf (0x%016llx)\n", off, (int64_t)off);
    printf("sens = %.2lf (0x%016llx)\n", sens, (int64_t)sens);
    printf("p = %.2lf (0x%x)\n\n", p / 100.0, (int32_t)(p / 100.0));

    // Second order temperature compensation
    if (temp < 2000.0)
    {
        double tmp;

        // Low temperature
        // T2 = 3 * dT^2 / 2^33
        // OFF2 = 61 * (TEMP - 2000)^2 / 2^4
        // SENS2 = 29 * (TEMP - 2000)^2 / 2^4
        t2 = 3.0 * dt * dt / pow(2, 33);
        tmp = (temp - 2000.0) * (temp - 2000.0) / pow(2, 4);
        off2 = 61.0 * tmp;
        sens2 = 29.0 * tmp;

        if (temp < -1500)
        {
            // Very low temperature
            // OFF2 = OFF2 + 17 * (TEMP + 1500)^2
            // SENS2 = SENS2 + 9 * (TEMP + 1500)^2
            tmp = (temp + 1500.0) * (temp + 1500.0);
            off2 += 17.0 * tmp;
            sens2 += 9.0 * tmp;
        }
    }
    else
    {
        // High temperature
        // T2 = 5 * dT^2 / 2^38
        // OFF2 = 0
        // SENS2 = 0
        t2 = 5.0 * dt * dt / pow(2, 38);
        off2 = 0.0;
        sens2 = 0.0;
    }

    temp -= t2;
    off -= off2;
    sens -= sens2;

    p = ((double)d1 * sens / pow(2, 21) - off) / pow(2, 15);

    printf("Second order temperature compensation:\n");
    printf("t2 = %.2lf (0x%016llx)\n", t2, (int64_t)t2);
    printf("off2 = %.2lf (0x%016llx)\n", off2, (int64_t)off2);
    printf("sens2 = %.2lf (0x%016llx)\n", sens2, (int64_t)sens2);
    printf("off = %.2lf (0x%016llx)\n", off, (int64_t)off);
    printf("sens = %.2lf (0x%016llx)\n", sens, (int64_t)sens);
    printf("temp = %.2lf (0x%x)\n", temp / 100.0, (int32_t)(temp / 100.0));
    printf("p = %.2lf (0x%x)\n", p / 100.0, (int32_t)(p / 100.0));

    return a.exec();
}



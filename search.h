typedef struct {
    uint8_t sn_gyro;
    uint8_t sn_us;
    uint8_t sn_color;
    uint8_t sn_touch;
} sensors_t;

typedef struct {
	uint8_t sn1;
	uint8_t sn2;
	uint8_t sn3;
	uint8_t sn4;
} motors_t;

extern void search();

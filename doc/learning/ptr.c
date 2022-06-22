typedef struct {
    double x;
    double y;
} EPOINT;

/* 平面上多边形（必须安顺序存放） */
typedef struct {
    int numpoint;
    EPOINT points[POLYGON_MAX_POINTS];
} EPOLYGON;


struct newtype {
    int count;
    char *name;
    EPOLYGON area;
} mytype[24];


    memset(mytype, 0x0, sizeof(mytype));
    mytype[12].count = 100;

    struct newtype *p = mytype;
    while (p->count != 100) {
        printf("count %d\n", p->count);

        p++;
    }

    printf("xxxxxxx\n");

    struct newtype **pa = (struct newtype **)mos_calloc(24, sizeof(struct newtype*));
    for (int i = 0; i < 24; i++) {
        pa[i] = (struct newtype *)mos_calloc(1, sizeof(struct newtype));
        pa[i]->count = i;
    }

    struct newtype **px = pa;
    while ((*px)->count < 12) {
        printf("count %d\n", (*px)->count);

        px++;
    }

    for (int i = 0; i < 24; i++) {
        mos_free(pa[i]);
    }
    mos_free(pa);

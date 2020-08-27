typedef struct 
{
  int x;	      /* Quadrant location of planet */
  int y;
  int pclass;   /* class M, N, or O (1, 2, or 3) */
  int crystals; /* has crystals */
  int known;    /* =1 contents known, =2 shuttle on this planet */
} PLANETS;

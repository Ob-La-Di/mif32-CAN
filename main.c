#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define COORDINATOR 0
#define BOOTSTRAPER 1

typedef enum {invitation, inv_accepted, integration, int_done, }  tag_dialog;


struct Zone{
  int x1, x2, y1, y2; // Coordonnées d'un espace
  int rank; // rang du processus
} typedef Zone;

struct Point{
  int x, y; //Coordinates of a point
  int rank; //Rank of the processus
} typedef Point;

struct Node{
  struct Node * next; // Liste chainée des tous les noeuds
  Zone * zone; //Zone qui correspond au noeud
} typedef Node;

struct Hash{
  Point * key; // Point.x + Point.y = key
  int value; // valeur du hash
} typedef Hash;

int main(int argc, char ** argv){
  int size, rank;
  MPI_Status status;
  MPI_Datatype mpi_point;
  Point * p;
  Zone * z;
	// Node* nodeList;
  // HashTable * table;
  // table = (Hash*) malloc(sizeof(Hash) * size);

  char fileName[20];
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Type_create_struct(3, (int[]){1,1,1},(MPI_Aint[]){offsetof(Point, x), offsetof(Point, y),offsetof(Point, rank)}, (MPI_Datatype[]){MPI_INT, MPI_INT, MPI_INT}, &mpi_point);
  MPI_Type_commit(&mpi_point);

  if(size <= 3){
    printf("Please run the program with at least 4 processus\n");
    return EXIT_FAILURE;
  }

  tag_dialog t;

  if(rank == COORDINATOR){

    t=invitation;
    for(int i=1;i < size; i++){
      MPI_Send(NULL, 0, MPI_INTEGER, i, t, MPI_COMM_WORLD); // we send invitation to every processus except 0
    }
    //
    // nodeList = (Node*) malloc(sizeof(Node));
		// nodeList->zone = (Zone*) malloc(sizeof(Zone));
		// nodeList->zone->rank = 1;
		// srand(time(NULL));


  }else{ //Everyone else

    t = invitation;
    MPI_Recv(NULL, 0, MPI_INTEGER, 0, t, MPI_COMM_WORLD, &status); //We retrieve the invitation from coordinator processus
    printf("I'm %d, coordinator just invited me\n", rank);
    p = (Point *) malloc(sizeof(Point));
    z = (Zone *) malloc(sizeof(Zone));
    p->x = rand()%1000;
    p->y = rand()%1000;
    p->rank = rank;

    if(rank == BOOTSTRAPER){ //Bootstraper processus gets the whole area
  		z->x1 = 0;
  		z->x2 = 1000;
  		z->y1 = 0;
  		z->y2 = 1000;
    }else{
      t = integration;
      MPI_Send(&p, 1, mpi_point, 1, t, MPI_COMM_WORLD);
      MPI_Recv(&p, 1, mpi_point, MPI_ANY_SOURCE, );
    }


    // table[rank].key = (Point *) malloc(sizeof(Point));
    // table[rank].key->x = rand()%1000;
    // table[rank].key->y = rand()%1000;
    // table[rank].key->rank = rank;
    // table[rank].value = table[rank].key->x + table[rank].key->y;



  }

  MPI_Type_free(&mpi_point);
  MPI_Finalize();
}

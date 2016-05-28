#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define COORDINATOR 0
#define BOOTSTRAPER 1

typedef enum
  {
    INVITATION, //Sent by coordinator at the beginning of lifecycle
    INV_ACCEPTED, //Sent by node processus once it's inserted
    INTEGRATION,
    INT_DONE
  } tag_dialog;


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

  if(rank == COORDINATOR){
    for(int i=1;i < size; i++){
      MPI_Send(NULL, 0, MPI_INT, i, INVITATION, MPI_COMM_WORLD); // we send an invitation to every processus except 0
      printf("coordinator, invitation to %d, sent", i);
      MPI_Recv(NULL, 0, MPI_INT, i, INV_ACCEPTED, MPI_COMM_WORLD, &status);
    }
  }else{ //Everyone else
    MPI_Recv(NULL, 0, MPI_INTEGER, 0, INVITATION, MPI_COMM_WORLD, &status); //We retrieve the invitation from coordinator processus
    printf("I'm %d, coordinator just invited me\n", rank);
    printf("%d", rank);
    p = (Point *) malloc(sizeof(Point));
    Point ptemp;
    z = (Zone *) malloc(sizeof(Zone));
    p->x = rand()%1000;
    p->y = rand()%1000;
    p->rank = rank;

    if(rank == BOOTSTRAPER){ //Bootstraper processus gets the whole area
      printf("I'm the Bootstraper, I take all the space");
  		z->x1 = 0;
  		z->x2 = 1000;
  		z->y1 = 0;
  		z->y2 = 1000;
    }else{
      MPI_Send(&p, 1, mpi_point, 1, INTEGRATION, MPI_COMM_WORLD);
      MPI_Recv(&p, 1, mpi_point, MPI_ANY_SOURCE, INT_DONE, MPI_COMM_WORLD, &status);
    }

    printf("%d, I notify coordinator", rank);
		MPI_Send(&p, 1, mpi_point, 0, INT_DONE, MPI_COMM_WORLD); //once integration is done, we send a notification to the processus 0

    while(1){
      MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

      switch(status.MPI_TAG){
        case INTEGRATION:
          MPI_Recv(&ptemp, 1, mpi_point, MPI_ANY_SOURCE, INTEGRATION, MPI_COMM_WORLD, &status);
          printf("switch 1");
          break;
      }
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

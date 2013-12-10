#include <mpi.h>
#include <stdio.h>

#define PLAQUE_SIZE 12
#define ID_COORDINATOR PLAQUE_SIZE
int main( int argc, char *argv[] ) {
	int i, signal;
	MPI_Status etat;
 
	
	// temperature de chacune de nos cases
	double plaque[PLAQUE_SIZE] = {20,30,25,26,
				      26,28,22,21,
                                      23,140,289,210
				     };
	double ambiant = 27;
	char *cmds[2] = { 
                "slave" ,
		"coordinator"
		
 	};

	int np[2] = { 
            PLAQUE_SIZE, // 12 slaves
                1 // 1 Coordinator 
	};
	
	// Pas d'info supplémentaire pour contrôler le lancement 
	MPI_Info infos[2] = { MPI_INFO_NULL, MPI_INFO_NULL };
	int errcodes[PLAQUE_SIZE];
	MPI_Comm intercomm; // L'espace de communication père - fils
 	
	MPI_Init( &argc, &argv );
 	
 	MPI_Comm_spawn_multiple ( 
		2, 
		cmds, 
		MPI_ARGVS_NULL, 
		np, 
		infos, 
		0, 
		MPI_COMM_WORLD, 
		&intercomm, 
		errcodes 
 	);
	printf ("Master : Coordinator created, %d slaves created \n", PLAQUE_SIZE);
	// Le père communique de façon synchrone avec chacun de
	// ses fils en utilisant l'espace de communication intercomm



	// Send Ambiant temperature to coordinator (ID = 0)
	MPI_Send (&ambiant,1,MPI_DOUBLE,12,0,intercomm);
	MPI_Recv(&signal,1, MPI_INT, 12, 0, intercomm, &etat);
	printf("Master : Coordinator [OK]\n");
	
	for (i=0; i<PLAQUE_SIZE; i++) {
		MPI_Send (&plaque[i],1,MPI_DOUBLE,i,0,intercomm);
		MPI_Recv(&signal, 1, MPI_INT,i, 0, intercomm, &etat);
		printf ("Master : Slave %d [OK] \n", i);
	}
	printf ("Master : initialization over ! \n");
	
	// LET'S BEGIN SIMULATION
	printf ("Master : Let's starting simulation ! \n");
	MPI_Send(&signal, 1, MPI_INT, 12, 0, intercomm);  // notify coordinator for simu	
	MPI_Recv(&signal, 1, MPI_INT, 12, 0, intercomm, &etat); // notify from coordinator, end simu
	printf("Master : End of Simulation \n");
	MPI_Finalize();
 	return 0;
 
}

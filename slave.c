#include <mpi.h>
#include <stdio.h>
#include <math.h>


double calcMoyenne(double temp, double temperature_near[], int nbr) {
    int i = 0;
    double tmp = 0;
    
    for(i = 0; i < nbr; i++) {
        tmp += temperature_near[i];
    }
    
    tmp += temp;
    tmp = tmp/(nbr+1);
    
    return tmp;
}

int main( int argc, char *argv[] ) {
	int signal, myrank, i, new_rank;
        int ranks[12] ={1,2,3,4,5,6,7,8,9,10,11,12};
        int width = 0, x, y;
        int indice[12];
        int iNbrNear = 0, iTmp = 0;
	double temperature;
	double ambiant;
        double temperature_near[8];
	
	MPI_Comm parent;
	MPI_Status etat;
	MPI_Init (&argc, &argv);

	MPI_Comm_get_parent (&parent);
	MPI_Comm_rank (MPI_COMM_WORLD,&myrank);
	// Creation d'un nouveau groupe de communication contenant seulement les esclaves.
	if (parent == MPI_COMM_NULL) {
		printf("Slave %d : no master here",myrank);
	} else {
            
            /** Init from master*/
 		MPI_Recv(&temperature, 1, MPI_DOUBLE, 0, 0, parent, &etat);
                MPI_Send(&signal, 1, MPI_INT, 0, 0, parent);
                /** end */    
                
		for (i=0;i<10;i++) {
                    iNbrNear = 0;
                    
                	MPI_Recv(&ambiant, 1, MPI_DOUBLE, 12, 0, MPI_COMM_WORLD, &etat);
                        //printf("Slave %d : Temperature received.\n", myrank);

                        MPI_Recv(&width, 1, MPI_INT, 12, 0, MPI_COMM_WORLD, &etat);
                        //printf("Slave %d : Width received. %i\n", myrank, width);

                        MPI_Recv(&signal, 1, MPI_INT, 12, 0, MPI_COMM_WORLD, &etat); // go
                        //printf("Slave %d : go !.\n", myrank);
                        
                        x = (int)floor(myrank % 4);
                        y = (int)floor(myrank / 4);
                        
                        if(y < 0)
                            y = 0;
                        
                        if(x < 0)
                            x = 0;
                        
                        //printf("[%i] x %i y %i\n", myrank, x, y);

                        for(i=1; i<9; i++) {
                            
                            iTmp = getIndice(x, y, width, i);
                            
                            if(iTmp > -1 && iTmp < 12) {
                                indice[iNbrNear] = iTmp;
                                //printf("[%i] Indice near slave : %i\n", myrank, iTmp);
                                iNbrNear ++;
                                MPI_Send(&temperature, 1, MPI_DOUBLE,iTmp,0,MPI_COMM_WORLD);
                            }
                        }
                        //printf("Slave %d : sended temperature to near %i slave !.\n", myrank, iNbrNear);
                        
                        for(i = 0; i < iNbrNear; i++) {
                            MPI_Recv(&temperature_near[i], 1, MPI_DOUBLE,indice[i],0,MPI_COMM_WORLD, &etat);
                            //printf("Slave %d : temperature received from slave (%i) !!.\n", myrank, indice[i]);
                        }
                        
                        temperature = calcMoyenne(temperature, temperature_near, iNbrNear);
                        //printf("Slave %d : Temperature %0.0f !.\n", myrank, temperature);
                	MPI_Send(&temperature, 1, MPI_DOUBLE,12,0,MPI_COMM_WORLD);
        	}

		
	}
	MPI_Finalize(); 
	return 0; 
 }



int getIndice(int x, int y, int width, int indiceRelatif) {
    int indice = -1;
    
    
        switch(indiceRelatif) {
            case 0: 
                if(x >= 0 && y >= 0 && x < width && y < 3)
                        indice = (y * width) + x;
            break;
            case 1:
                indice = getIndice(x, y-1, width, 0);
            break;
            case 2:
                indice = getIndice(x+1, y-1, width, 0);
            break;
            case 3:
                indice = getIndice(x+1, y, width, 0);
            break;
            case 4:
                indice = getIndice(x+1, y+1, width, 0);
            break;
            case 5:
                indice = getIndice(x, y+1, width, 0);
            break;
            case 6:
                indice = getIndice(x-1, y+1, width, 0);
            break;
            case 7:
                indice = getIndice(x-1, y, width, 0);
            break;
            case 8:
                indice = getIndice(x-1, y-1, width, 0);
            break;
        }
    
    
    return indice;
}
#include "fiber.h"
#include "tridag.h"

Fiber::Fiber(int size) {
    unsigned int i = nodes.size();
    nodes.resize(size, NULL);
    B.resize(size +1);
    for(;i < nodes.size(); i++) {
        nodes[i] = new Node();
        B[i] = 0;
    }
    B[size] = 0;
}
Fiber::~Fiber() {}
//#############################################################
// Solve PDE for Vm along fiber using tridiagonal solver.
//#############################################################
void Fiber::updateVm(double& dt) {
    int i;
    int nn = nodes.size();
    if(nn <= 1) { return;}

    for(i=0;i<nn;i++){
        nodes[i]->d1 = B[i]*dt;
        nodes[i]->d2 = -(B[i]*dt+B[i+1]*dt+2);
        nodes[i]->d3 = B[i+1]*dt;
        if(i>0&&i<(nn-1))
            nodes[i]->r = -B[i]*dt*nodes[i-1]->cell->vOld+(B[i]*dt+B[i+1]*dt-2)*nodes[i]->cell->vOld-B[i+1]*dt*nodes[i+1]->cell->vOld+dt/nodes[i]->cell->Cm*(nodes[i]->cell->iTotold+nodes[i]->cell->iTot);
    }
    nodes[0]->r = (B[1]*dt-2)*nodes[0]->cell->vOld-B[1]*dt*nodes[1]->cell->vOld+dt/nodes[0]->cell->Cm*(nodes[0]->cell->iTotold+nodes[0]->cell->iTot);
    nodes[nn-1]->r = -B[nn-1]*dt*nodes[nn-2]->cell->vOld+(B[nn-1]*dt-2)*nodes[nn-1]->cell->vOld+dt/nodes[nn-1]->cell->Cm*(nodes[nn-1]->cell->iTotold+nodes[nn-1]->cell->iTot);

    tridag(nodes);
   
    for(i=0;i<nn;i++){
        nodes[i]->cell->iTotold=nodes[i]->cell->iTot;
        nodes[i]->cell->dVdt=(nodes[i]->vNew-nodes[i]->cell->vOld)/dt;
        //##### Conservation for multicellular fiber ############
        nodes[i]->dIax=-(nodes[i]->cell->dVdt+nodes[i]->cell->iTot);
        nodes[i]->cell->iKt=nodes[i]->cell->iKt+nodes[i]->dIax;
        nodes[i]->cell->setV(nodes[i]->vNew);
   }
}
/*
//#############################################################
// Dynamic time step for one-dimensional fiber.
//#############################################################
int Fiber::tstep()
{
  int i,j;
  int vmflag=0;
  
  for(i=0;i<nn;i++){
     nodes[i]->cell->t=nodes[i]->cell->t+dt;
     nodes[i]->cell->dt=dt;
     if(nodes[i]->cell->dVdt>1.0||(nodes[i]->cell->t>(nodes[i]->cell->stimt-1.0)&&nodes[i]->cell->t<nodes[i]->cell->stimt))
	vmflag=2;
     else if(nodes[i]->cell->vOld>-50&&vmflag!=2)
        vmflag=1;
  }	
  
  if(vmflag==2)
      dt=dtmin;
  else if(vmflag==1)
      dt=dtmed;
  else
      dt=dtmax;
     
  if(nodes[0]->cell->t>tMax)
     return 0;
  else
     return 1;
}*/

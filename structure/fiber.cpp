#include "fiber.h"
#include "tridag.h"

Fiber::Fiber(int size) {
    unsigned int i = static_cast<int>(nodes.size());
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
    int nn = static_cast<int>(nodes.size());
    if(nn <= 1) { return;}

    for(i=0;i<nn;i++){
        nodes[i]->d1 = B[i]*dt;
        nodes[i]->d2 = -(B[i]*dt+B[i+1]*dt+2);
        nodes[i]->d3 = B[i+1]*dt;
        if(i>0&&i<(nn-1))
            nodes[i]->r = -B[i]*dt*nodes[i-1]->cell->par("vOld")+(B[i]*dt+B[i+1]*dt-2)*nodes[i]->cell->par("vOld")-B[i+1]*dt*nodes[i+1]->cell->par("vOld")+dt/nodes[i]->cell->par("Cm")*(nodes[i]->cell->par("iTotold")+nodes[i]->cell->par("iTot"));
    }
    nodes[0]->r = (B[1]*dt-2)*nodes[0]->cell->par("vOld")-B[1]*dt*nodes[1]->cell->par("vOld")+dt/nodes[0]->cell->par("Cm")*(nodes[0]->cell->par("iTotold")+nodes[0]->cell->par("iTot"));
    nodes[nn-1]->r = -B[nn-1]*dt*nodes[nn-2]->cell->par("vOld")+(B[nn-1]*dt-2)*nodes[nn-1]->cell->par("vOld")+dt/nodes[nn-1]->cell->par("Cm")*(nodes[nn-1]->cell->par("iTotold")+nodes[nn-1]->cell->par("iTot"));

    tridag(nodes);
   
    for(i=0;i<nn;i++){
        nodes[i]->cell->setPar("iTotold",nodes[i]->cell->par("iTot"));
        nodes[i]->cell->setPar("dVdt",(nodes[i]->vNew-nodes[i]->cell->par("vOld"))/dt);
        //##### Conservation for multicellular fiber ############
        nodes[i]->dIax=-(nodes[i]->cell->par("dVdt")+nodes[i]->cell->par("iTot"));
        nodes[i]->cell->setPar("iKt",nodes[i]->cell->par("iKt")+nodes[i]->dIax);
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

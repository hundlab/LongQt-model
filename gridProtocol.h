// 5/10/2016
//################################################
//protocol for a grid off cells
//class declarations
//################################################

#ifndef GRIDPROTOCOL_H
#define GRIDPROTOCOL_H

class gridProtocol : public Protocol {
  public:
    gridProtocol();
    gridProtocol(const gridProtocol& toCopy);
    gridProtocol* clone();
    gridProtocol& operator=(const gridProtocol& toCopy);

    bool runTrial() override;
  private:
    void CCcopt(const gridProtocol& toCopy);
};
#endif

/* 5/10/2016
 *
 * a 2D grid of cells of various types
 * class declaration
 */
#ifndef CELLGRID_H
#define CELLGRID_H

#include <array>
#include <cmath>
#include <iterator>
#include <list>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include "cellutils.h"
#include "fiber.h"
namespace LongQt {

class Grid {
 public:
  Grid();
  Grid(const Grid& other);
  ~Grid();

  class GridIterator {
    using value_type = std::shared_ptr<Node>;
    using reference = value_type&;
    using pointer = value_type*;
    using difference_type = std::pair<int, int>;
    using iterator_category = std::bidirectional_iterator_tag;

    int row = 0;
    int col = 0;
    Grid* parent = 0;
    bool rowsFirst = true;

    GridIterator(Grid* parent, bool rowsFirst = true);

    friend class Grid;

   public:
    GridIterator(const GridIterator& o)
        : row(o.row), col(o.col), parent(o.parent), rowsFirst(o.rowsFirst) {}

    GridIterator& operator++();
    GridIterator operator++(int);
    GridIterator& operator--();
    GridIterator operator--(int);
    GridIterator& operator+=(int i);
    GridIterator& operator-=(int i);
    GridIterator operator+(int i);
    GridIterator operator-(int i);

    std::shared_ptr<Node> operator*();

    bool operator==(const GridIterator& rhs) const;
    bool operator!=(const GridIterator& rhs) const;
//    bool operator==(const Fiber::FiberIterator& rhs) const;
  };

  Grid& operator=(const Grid&) = delete;

  //	inline virtual edge(int x, int y, CellUtils::Side s);
  virtual void addRow();
  virtual void addRows(unsigned int num);
  virtual void addColumn();
  virtual void addColumns(unsigned int num);
  virtual void removeRow(int pos);
  virtual void removeRows(unsigned int num, int position = 0);
  virtual void removeColumn(int pos);
  virtual void removeColumns(unsigned int num, int position = 0);
  //  virtual void setCellTypes(std::list<CellInfo>& cells);
  //  virtual void setCellTypes(const CellInfo& singleCell);
  virtual int rowCount() const;
  virtual int columnCount() const;
  virtual std::pair<int, int> findNode(const std::shared_ptr<Node> toFind);
  virtual std::shared_ptr<Node> operator()(const std::pair<int, int>& p);
  virtual std::shared_ptr<Node> operator()(const int row, const int col);
  virtual void reset();
  //  virtual void updateB(CellInfo node, CellUtils::Side s);
  void updateNodePositions();
  virtual void setup();

  //  virtual const_iterator begin() const;
  //  virtual const_iterator end() const;
  virtual GridIterator begin();
  virtual GridIterator end();
  virtual GridIterator beginRowsFirst();
  virtual GridIterator beginColumnsFirst();

  std::vector<Fiber> rows;
  std::vector<Fiber> columns;

  bool iterRowsFirst = true;
  double dx = 0.01;
  double dy = 0.01;
  //Having multiple patches per node is not supported and has been commented out
//  int np = 1;  // nodes per patch
};
}  // namespace LongQt

#endif

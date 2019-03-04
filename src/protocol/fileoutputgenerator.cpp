#include "fileoutputgenerator.h"

#include "logger.h"
using namespace LongQt;

int FileOutputHandler::max_fileHandlers = 500;
int FileOutputHandler::buffmax = 10000;

std::atomic<FileOutputHandler::Props*> FileOutputHandler::head = 0;
std::atomic<int> FileOutputHandler::numUsed = 0;

// synchronous
FileOutputHandler::FileOutputHandler() {}
// synchronous
FileOutputHandler::~FileOutputHandler() { this->close(); }

void FileOutputHandler::clear() {
  this->buffer.clear();
  this->prop.buffsize = 0;
}

void FileOutputHandler::close() {
  while (!prop.writing.test_and_set())
    ;
  if (this->buffer.size() > 0) {
    while (!this->openStream()) {
    }
    this->writeAll();
  }
  if (prop.open) {
    this->remove(&prop);
  }
  this->closeStream(&prop);
  prop.writing.clear();
}

// only open a stream if buffer > buffmax
bool FileOutputHandler::openStream() {
  while (!prop.writing.test_and_set())
    ;
  if (prop.open) {
    return prop.open;
  }

  int used = ++numUsed;
  if (used >= max_fileHandlers) {
    --numUsed;
    auto toKill = this->find_min();
    // begin marauding
    int count = 0;
    while (count < max_fileHandlers &&
           (toKill || !toKill->writing.test_and_set())) {
      toKill = toKill && toKill->next ? toKill->next : this->head;
      ++count;
    }
    if (count >= max_fileHandlers) {
      // give up
      prop.writing.clear();
      return prop.open;
    }
    this->remove(toKill);
    this->closeStream(toKill);
    toKill->writing.clear();
  }
  prop.open = true;
  prop.stream.open(this->filename, std::ios_base::app);
  prop.stream.precision(10);
  if (!prop.stream.good()) {
    prop.stream.close();
    Logger::getInstance()->write<std::runtime_error>("IOBase: Error Opening {}",
                                                     filename);
    --numUsed;
    prop.open = false;
    prop.writing.clear();
    return prop.open;
  }
  this->push_front(&prop);
  return prop.open;
}

void FileOutputHandler::writeAll() {
  while (buffer.size() > 0) {
    prop.stream << buffer.front();
    buffer.pop_front();
  }
  prop.writing.clear();
}

// requires writing to be locked
void FileOutputHandler::closeStream(Props* p) {
  if (p->stream.is_open()) {
    p->stream.flush();
    p->stream.close();
  }
  p->open = false;
}

void FileOutputHandler::push_front(FileOutputHandler::Props* p) {
  Props* ourHead = head.load();
  prop.next = ourHead;
  while (
      !head.compare_exchange_weak(ourHead, &prop, std::memory_order_relaxed)) {
    prop.next = ourHead;
  }
}

// requires writing to be locked
void FileOutputHandler::remove(FileOutputHandler::Props* p) {
  Props* ourHead = head;
  if (ourHead == p) {
    if (head.compare_exchange_strong(ourHead, p->next)) {
      return;
    }
  }
  Props* curr;
  do {
    curr = head;
    while (curr->next && curr->next != p) {
      curr = curr->next;
    }
    if (curr->next != p) {
      Logger::getInstance()->write("FileOutputHandler: file missing from list");
      p->open = false;
      return;
    }
    while (!curr->writing.test_and_set())
      ;
    if (!curr->open) continue;
    curr->next.store(p->next.load());
    curr->writing.clear();
  } while (false);
}

FileOutputHandler::Props* FileOutputHandler::find_min() {
  Props* min = 0;
  do {
    Props* curr = head;
    int min_val = std::numeric_limits<int>::max();
    while (curr) {
      if (curr->buffsize < min_val) {
        min_val = curr->buffsize;
        min = curr;
      }
      curr = curr->next;
    }
  } while (head && min && !min->open);
  return min;
}

//
// Generated file, do not edit! Created by nedtool 5.5 from messages/transmissionIndication.msg.
//

#ifndef __TRANSMISSIONINDICATION_M_H
#define __TRANSMISSIONINDICATION_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0505
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



/**
 * Class generated from <tt>messages/transmissionIndication.msg:19</tt> by nedtool.
 * <pre>
 * //
 * // TODO generated message class
 * //
 * packet transmissionIndication
 * {
 * }
 * </pre>
 */
class transmissionIndication : public ::omnetpp::cPacket
{
  protected:

  private:
    void copy(const transmissionIndication& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const transmissionIndication&);

  public:
    transmissionIndication(const char *name=nullptr, short kind=0);
    transmissionIndication(const transmissionIndication& other);
    virtual ~transmissionIndication();
    transmissionIndication& operator=(const transmissionIndication& other);
    virtual transmissionIndication *dup() const override {return new transmissionIndication(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const transmissionIndication& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, transmissionIndication& obj) {obj.parsimUnpack(b);}


#endif // ifndef __TRANSMISSIONINDICATION_M_H


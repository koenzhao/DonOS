#ifndef _DOS_INTERRUPT_H_
#define _DOS_INTERRUPT_H_

void triggerPendSVC(void);
extern void DOS_DisableIrq(void);
extern void DOS_EnableIrq(void);

#endif

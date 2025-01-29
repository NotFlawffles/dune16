#ifndef DUNE16_H
#define DUNE16_H

#include <bits/pthreadtypes.h>

#include <dync/common/types.h>

#include "memory.h"
#include "instruction/instruction.h"

struct dune16_t {
    u16 *memory;
    u8 flags;
    u8 interrupts;

    struct {
	u16 a, b, c, d, pc;
    } registers;

    struct instruction_t instruction;

    struct {
	pthread_t irq, pc_exceed;
    } threads;

    bool should_terminate;
};

struct dune16_t dune16_new(void);

u16 dune16_read_16(struct dune16_t *const this, const u16 address);
u32 dune16_read_32(struct dune16_t *const this, const u16 address);

void dune16_write_16(struct dune16_t *const this, const u16 address, const u16 value);

u16 *dune16_get_register(struct dune16_t *const this, const u8 index);

void dune16_load_program_from_file(struct dune16_t *const this, char *const path);
void dune16_load_program_from_memory(struct dune16_t *const this, u16 *const program, const usize program_size);
void dune16_run(struct dune16_t *const this);

#endif // DUNE16_H

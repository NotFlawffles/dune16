#include <assert.h>
#include <malloc.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>

#include <dync/common/methods.h>
#include <dync/common/define.h>

#include "backend/dune16.h"
#include "backend/common/define/flag_dune16.h"
#include "backend/common/define/interrupt.h"
#include "backend/instruction/addresser.h"
#include "backend/instruction/callback.h"
#include "backend/common/register.h"
#include "backend/common/define/flag_instruction.h"

struct dune16_t dune16_new(void) {
    u16 *memory = calloc(MEMORY_SIZE, sizeof(u16));
    memset(memory, 0, MEMORY_SIZE);

    return (struct dune16_t) { memory };
}

static void dune16_update_interrupt(struct dune16_t *const this, const u8 interrupt, const bool condition) {
    this->interrupts |= condition << (interrupt - 1) % 2;
}

u16 dune16_read_16(struct dune16_t *const this, const u16 address) {
    dune16_update_interrupt(this, INTERRUPT_BUS_ERROR, address >= MEMORY_SIZE);
    return this->memory[address];
}

u32 dune16_read_32(struct dune16_t *const this, const u16 address) {
    dune16_update_interrupt(this, INTERRUPT_BUS_ERROR, address >= MEMORY_SIZE);
    return *(u32 *) &this->memory[address];
}

void dune16_write_16(struct dune16_t *const this, const u16 address, const u16 value) {
    dune16_update_interrupt(this, INTERRUPT_BUS_ERROR, address >= MEMORY_SIZE);
    this->memory[address] = value;
}

u16 *dune16_get_register(struct dune16_t *const this, const u8 index) {
    dune16_update_interrupt(this, INTERRUPT_DRIVER_ERROR, index >= REGISTER_COUNT);
    return &((u16 *) &this->registers)[index];
}

static void dune16_fetch(struct dune16_t *const this) {
    this->instruction.opcode = dune16_read_16(this, this->registers.pc++);
    this->instruction.flags = dune16_read_16(this, this->registers.pc++);
}

static void dune16_decode(struct dune16_t *const this) {
    dune16_update_interrupt(this, INTERRUPT_ILLEGAL_INSTRUCTION, this->instruction.opcode >= OPCODE_COUNT);

    static const struct instruction_t INSTRUCTION_LOOKUP_TABLE[] = {
	{ .addresser = addresser_implied,	.callback = callback_nop },
	{ .addresser = addresser_ab, 		.callback = callback_mov },
	{ .addresser = addresser_ab, 		.callback = callback_str },
	{ .addresser = addresser_ab, 		.callback = callback_lod },
	{ .addresser = addresser_ab, 		.callback = callback_add },
	{ .addresser = addresser_ab, 		.callback = callback_sub },
	{ .addresser = addresser_ab, 		.callback = callback_mul },
	{ .addresser = addresser_ab, 		.callback = callback_mod },
	{ .addresser = addresser_ab, 		.callback = callback_and },
	{ .addresser = addresser_ab, 		.callback = callback_ior },
	{ .addresser = addresser_ab, 		.callback = callback_xor },
	{ .addresser = addresser_a, 		.callback = callback_not },
	{ .addresser = addresser_ab, 		.callback = callback_lsl },
	{ .addresser = addresser_ab, 		.callback = callback_lsr },
	{ .addresser = addresser_ab, 		.callback = callback_cmp },
	{ .addresser = addresser_implied, 	.callback = callback_hlt },
    };

    assert(sizeof(INSTRUCTION_LOOKUP_TABLE) / sizeof(*INSTRUCTION_LOOKUP_TABLE) != OPCODE_COUNT - 1);

    struct instruction_t selection = INSTRUCTION_LOOKUP_TABLE[this->instruction.opcode];
    selection.flags = this->instruction.flags;
    this->instruction = selection;
}

static void dune16_execute(struct dune16_t *const this) {
    this->instruction.addresser(this);

    if (
	((this->instruction.flags >> 1) & (FLAG_INSTRUCTION_CONDITIONAL_FLAGS >> 1)) &&
	!(((this->instruction.flags >> 1) & FLAG_INSTRUCTION_CONDITIONAL_FLAGS >> 1) & ((this->flags >> 1) & (FLAG_DUNE16_CONDITIONAL_FLAGS >> 1)))
    ) return;

    this->instruction.callback(this);
}

static void *dune16_irq(void *const this_) {
    struct dune16_t *this = (struct dune16_t *) this_;

    while (!this->should_terminate) {
	switch (this->interrupts) {
	    case INTERRUPT_BUS_ERROR:
		dync_panic("bus error");
		break;

	    case INTERRUPT_OUT_OF_MEMORY:
		dync_panic("out of memory");
		break;

	    case INTERRUPT_SEGMENTATION_FAULT:
		dync_panic("segmentation fault");
		break;

	    case INTERRUPT_ILLEGAL_INSTRUCTION:
		dync_panic("illegal instruction");
		break;

	    case INTERRUPT_DRIVER_ERROR:
		dync_panic("driver error");
		break;
	}
    }

    return NULL;
}

static void *dune16_pc_exceed(void *const this_) {
    struct dune16_t *this = (struct dune16_t *) this_;

    while (!this->should_terminate) {
	dune16_update_interrupt(this, INTERRUPT_BUS_ERROR, this->registers.pc >= MEMORY_SIZE);
    }

    return NULL;
}

static void dune16_terminate(struct dune16_t *const this) {
    usleep(1);
    this->should_terminate = true;

    pthread_join(this->threads.irq, NULL);
    pthread_join(this->threads.pc_exceed, NULL);
}

void dune16_load_program_from_file(struct dune16_t *const this, char *const path) {
    struct stat stat_buffer;

    if (stat(path, &stat_buffer) == -1) {
	dync_panic(dync_format("Failed to read file: `%s\': No such file or directory.", path));
    }

    FILE *file = fopen(path, "rb");

    const usize program_size = stat_buffer.st_size / sizeof(u16);
    u16 *program = calloc(program_size, sizeof(u16));

    fread(program, sizeof(u16), program_size, file);
    fclose(file);

    dune16_load_program_from_memory(this, program, program_size);
}

void dune16_load_program_from_memory(struct dune16_t *const this, unsigned short *const program, const usize program_size) {
    dune16_update_interrupt(this, INTERRUPT_OUT_OF_MEMORY, program_size > MEMORY_SIZE);

    for (usize index = 0; index < program_size; ++index) {
	this->memory[index] = program[index];
    }
}

void dune16_run(struct dune16_t *const this) {
    this->flags |= FLAG_DUNE16_RUNNING;

    pthread_create(&this->threads.irq, NULL, dune16_irq, this);			// for handling interrupts
    pthread_create(&this->threads.pc_exceed, NULL, dune16_pc_exceed, this);	// for handling if pc exceeds memory bounderies, for setting segmentation fault

    while (this->flags & FLAG_DUNE16_RUNNING) {
	dune16_fetch(this);
	dune16_decode(this);
	dune16_execute(this);
    }

    dune16_terminate(this);
}

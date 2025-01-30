#ifndef DUNE16_CALLBACK_H
#define DUNE16_CALLBACK_H

struct dune16_t;

void callback_nop(struct dune16_t *const dune16);
void callback_mov(struct dune16_t *const dune16);
void callback_str(struct dune16_t *const dune16);
void callback_lod(struct dune16_t *const dune16);
void callback_add(struct dune16_t *const dune16);
void callback_sub(struct dune16_t *const dune16);
void callback_mul(struct dune16_t *const dune16);
void callback_mod(struct dune16_t *const dune16);
void callback_and(struct dune16_t *const dune16);
void callback_ior(struct dune16_t *const dune16);
void callback_xor(struct dune16_t *const dune16);
void callback_not(struct dune16_t *const dune16);
void callback_lsl(struct dune16_t *const dune16);
void callback_lsr(struct dune16_t *const dune16);
void callback_cmp(struct dune16_t *const dune16);
void callback_bch(struct dune16_t *const dune16);
void callback_sys(struct dune16_t *const dune16);
void callback_hlt(struct dune16_t *const dune16);

#endif // DUNE16_CALLBACK_H

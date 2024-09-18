/*
*    ------ BEGIN LICENSE ATTRIBUTION ------
*    
*    Portions of this file have been appropriated or derived from the following project(s) and therefore require attribution to the original licenses and authors.
*    
*    Repository: https://github.com/torvalds/linux
*    Source File: arch/x86/kernel/head64.c
*    Licenses:
*      GNU General Public License v2.0 only
*      SPDXId: GPL-2.0-only
*    
*    Auto-attribution by Threatrix, Inc.
*    
*    ------ END LICENSE ATTRIBUTION ------
*/
unsigned long __head __startup_64(unsigned long physaddr,
				  struct boot_params *bp)
{
	pmd_t (*early_pgts)[PTRS_PER_PMD] = RIP_REL_REF(early_dynamic_pgts);
	unsigned long pgtable_flags;
	unsigned long load_delta;
	pgdval_t *pgd;
	p4dval_t *p4d;
	pudval_t *pud;
	pmdval_t *pmd, pmd_entry;
	bool la57;
	int i;

	la57 = check_la57_support();

	/* Is the address too large? */
	if (physaddr >> MAX_PHYSMEM_BITS)
		for (;;);

	/*
	 * Compute the delta between the address I am compiled to run at
	 * and the address I am actually running at.
	 */
	load_delta = physaddr - (unsigned long)(_text - __START_KERNEL_map);
	RIP_REL_REF(phys_base) = load_delta;

	/* Is the address not 2M aligned? */
	if (load_delta & ~PMD_MASK)
		for (;;);

	/* Include the SME encryption mask in the fixup value */
	load_delta += sme_get_me_mask();

	/* Fixup the physical addresses in the page table */

	pgd = &RIP_REL_REF(early_top_pgt)->pgd;
	pgd[pgd_index(__START_KERNEL_map)] += load_delta;

	if (la57) {
		p4d = (p4dval_t *)&RIP_REL_REF(level4_kernel_pgt);
		p4d[MAX_PTRS_PER_P4D - 1] += load_delta;

		pgd[pgd_index(__START_KERNEL_map)] = (pgdval_t)p4d | _PAGE_TABLE;
	}

	RIP_REL_REF(level3_kernel_pgt)[PTRS_PER_PUD - 2].pud += load_delta;
	RIP_REL_REF(level3_kernel_pgt)[PTRS_PER_PUD - 1].pud += load_delta;

	for (i = FIXMAP_PMD_TOP; i > FIXMAP_PMD_TOP - FIXMAP_PMD_NUM; i--)
		RIP_REL_REF(level2_fixmap_pgt)[i].pmd += load_delta;

	/*
	 * Set up the identity mapping for the switchover.  These
	 * entries should *NOT* have the global bit set!  This also
	 * creates a bunch of nonsense entries but that is fine --
	 * it avoids problems around wraparound.
	 */

	pud = &early_pgts[0]->pmd;
	pmd = &early_pgts[1]->pmd;
	RIP_REL_REF(next_early_pgt) = 2;

	pgtable_flags = _KERNPG_TABLE_NOENC + sme_get_me_mask();

	if (la57) {
		p4d = &early_pgts[RIP_REL_REF(next_early_pgt)++]->pmd;

		i = (physaddr >> PGDIR_SHIFT) % PTRS_PER_PGD;
		pgd[i + 0] = (pgdval_t)p4d + pgtable_flags;
		pgd[i + 1] = (pgdval_t)p4d + pgtable_flags;

		i = physaddr >> P4D_SHIFT;
		p4d[(i + 0) % PTRS_PER_P4D] = (pgdval_t)pud + pgtable_flags;
		p4d[(i + 1) % PTRS_PER_P4D] = (pgdval_t)pud + pgtable_flags;
	} else {
		i = (physaddr >> PGDIR_SHIFT) % PTRS_PER_PGD;
		pgd[i + 0] = (pgdval_t)pud + pgtable_flags;
		pgd[i + 1] = (pgdval_t)pud + pgtable_flags;
	}

	i = physaddr >> PUD_SHIFT;
	pud[(i + 0) % PTRS_PER_PUD] = (pudval_t)pmd + pgtable_flags;
	pud[(i + 1) % PTRS_PER_PUD] = (pudval_t)pmd + pgtable_flags;

	pmd_entry = __PAGE_KERNEL_LARGE_EXEC & ~_PAGE_GLOBAL;
	/* Filter out unsupported __PAGE_KERNEL_* bits: */
	pmd_entry &= RIP_REL_REF(__supported_pte_mask);
	pmd_entry += sme_get_me_mask();
	pmd_entry +=  physaddr;

	for (i = 0; i < DIV_ROUND_UP(_end - _text, PMD_SIZE); i++) {
		int idx = i + (physaddr >> PMD_SHIFT);

		pmd[idx % PTRS_PER_PMD] = pmd_entry + i * PMD_SIZE;
	}

	/*
	 * Fixup the kernel text+data virtual addresses. Note that
	 * we might write invalid pmds, when the kernel is relocated
	 * cleanup_highmap() fixes this up along with the mappings
	 * beyond _end.
	 *
	 * Only the region occupied by the kernel image has so far
	 * been checked against the table of usable memory regions
	 * provided by the firmware, so invalidate pages outside that
	 * region. A page table entry that maps to a reserved area of
	 * memory would allow processor speculation into that area,
	 * and on some hardware (particularly the UV platform) even
	 * speculative access to some reserved areas is caught as an
	 * error, causing the BIOS to halt the system.
	 */

	pmd = &RIP_REL_REF(level2_kernel_pgt)->pmd;

	/* invalidate pages before the kernel image */
	for (i = 0; i < pmd_index((unsigned long)_text); i++)
		pmd[i] &= ~_PAGE_PRESENT;

	/* fixup pages that are part of the kernel image */
	for (; i <= pmd_index((unsigned long)_end); i++)
		if (pmd[i] & _PAGE_PRESENT)
			pmd[i] += load_delta;

	/* invalidate pages after the kernel image */
	for (; i < PTRS_PER_PMD; i++)
		pmd[i] &= ~_PAGE_PRESENT;

	return sme_postprocess_startup(bp, pmd);
}

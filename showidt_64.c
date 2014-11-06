#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <asm/desc.h>
#include <asm/fixmap.h>
#include <linux/jump_label.h>
#include <linux/types.h>
#include <asm/pgtable.h>
#include <asm/pgtable_64.h>
#include <asm/tlbflush.h>
#include <asm/current.h>
MODULE_LICENSE("GPL");
//jeremy
struct desc_ptr idtr;
int i;
gate_desc *idt_table2;
unsigned long addr;

int init_module(void)
{
asm ("sidt idtr\n\t");
printk("\ninit_module by Jeremy : showidt\n");
asm ("sidt idtr\n\t");
printk("IDTR=>limit:0x%04x\nbase:0x%016lx\n",idtr.size,idtr.address);

idt_table2 = ((gate_desc *) idtr.address);
//start modifing the fixmap
printk("idtr.address : %lx\n", idtr.address);
pgd_t *pgd;
pud_t *pud;
pmd_t *pmd;
pte_t *pte;
pgd = pgd_offset(current->mm, idtr.address);
if (pgd_none(*pgd)) {
            printk(KERN_ERR
                    "PGD FIXMAP MISSING, it should be setup in head.S!\n");
            return;
}
pud = pud_offset(pgd, idtr.address);
pmd = pmd_offset(pud, idtr.address);
pte = pte_offset_kernel(pmd, idtr.address);

printk("pte : %lx\n", pte);
//printk("pte_write(%lx):%x\n", *pte, pte_write(*pte));
//*(unsigned long *)pte ^= 2UL;
printk("pte_write(%lx):%x\n", *pte, pte_write(*pte));
//printk("PTE_FLAGS_MASK : %lx\n", (~((pteval_t)(((signed long)(~(((1UL) << 12)-1))) & ((phys_addr_t)((1ULL << 46) - 1))))) );
__flush_tlb_one(idtr.address);
//end
gate_desc * ggla;
ggla = (gate_desc *) idtr.address;
printk("*0x%lx (high_offset)= %08x\n", (char*)ggla+8,*((char*)ggla+8) );
printk("*0x%lx (low_offset)=%04x\n",ggla,*(unsigned short*)ggla);
printk("*0x%lx (middle_offset)=%04x\n",(char*)ggla+4,ggla->offset_middle);
/*
ggla = (gate_desc *)fix_to_virt(FIX_RO_IDT);
printk("*0x%lx(high_offset)= %08x\n",(char*)ggla+8,ggla->offset_high);
printk("*0x%lx(low_offset)= %04x\n",ggla,ggla->offset_low);
printk("*0x%lx(middle_offset)= %04x\n",(char*)ggla+4,ggla->offset_middle );
*/
//detect exist FIX_RO_IDT ?
if ( __virt_to_fix(idtr.address) == FIX_RO_IDT )
        printk("FIX_RO_IDT(%d) is enable\n", FIX_RO_IDT);


for(i=0;i<3;i++){
printk("Interrupt:0x%02x(%03d) Offset:0x%08x%x%x Selector:0x%04x Attr:0x%04x\n",i,i,idt_table2[i].offset_high,
idt_table2[i].offset_middle,idt_table2[i].offset_low,idt_table2[i].segment,(idt_table2[i].p << 15)|(idt_table2[i].dpl <<13)|(idt_table2[i].type << 8)|(idt_table2[i].zero0 << 3)|(idt_table2[i].ist));
}
return 0;
}
void cleanup_module(void)
{
printk("\nclean_module : showidt\n\n");
}


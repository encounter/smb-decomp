/* 8000D5E4 00009504  7C 08 02 A6 */	mflr r0
/* 8000D5E8 00009508  3C 60 80 17 */	lis r3, eventInfo@ha
/* 8000D5EC 0000950C  90 01 00 04 */	stw r0, 4(r1)
/* 8000D5F0 00009510  38 63 3C C8 */	addi r3, r3, eventInfo@l
/* 8000D5F4 00009514  94 21 FF 28 */	stwu r1, -0xd8(r1)
/* 8000D5F8 00009518  DB E1 00 D0 */	stfd f31, 0xd0(r1)
/* 8000D5FC 0000951C  DB C1 00 C8 */	stfd f30, 0xc8(r1)
/* 8000D600 00009520  93 E1 00 C4 */	stw r31, 0xc4(r1)
/* 8000D604 00009524  93 C1 00 C0 */	stw r30, 0xc0(r1)
/* 8000D608 00009528  88 03 01 20 */	lbz r0, 0x120(r3)
/* 8000D60C 0000952C  3C 60 80 1F */	lis r3, lbl_801EEC80@ha
/* 8000D610 00009530  3B E3 EC 80 */	addi r31, r3, lbl_801EEC80@l
/* 8000D614 00009534  2C 00 00 02 */	cmpwi r0, 2
/* 8000D618 00009538  41 82 08 B0 */	beq lbl_8000DEC8
/* 8000D61C 0000953C  3C 60 80 1F */	lis r3, lbl_801F3A58@ha
/* 8000D620 00009540  38 63 3A 58 */	addi r3, r3, lbl_801F3A58@l
/* 8000D624 00009544  A8 03 00 04 */	lha r0, 4(r3)
/* 8000D628 00009548  2C 00 00 00 */	cmpwi r0, 0
/* 8000D62C 0000954C  41 81 00 08 */	bgt lbl_8000D634
/* 8000D630 00009550  48 00 08 98 */	b lbl_8000DEC8
lbl_8000D634:
/* 8000D634 00009554  38 60 00 02 */	li r3, 2
/* 8000D638 00009558  48 06 32 45 */	bl g_find_sprite_with_probably_not_font
/* 8000D63C 0000955C  28 03 00 00 */	cmplwi r3, 0
/* 8000D640 00009560  40 82 00 10 */	bne lbl_8000D650
/* 8000D644 00009564  C3 E2 80 E4 */	lfs f31, 0.0f  //lbl_802F28E4  //-_SDA2_BASE_(r2)
/* 8000D648 00009568  FF C0 F8 90 */	fmr f30, f31
/* 8000D64C 0000956C  48 00 00 28 */	b lbl_8000D674
lbl_8000D650:
/* 8000D650 00009570  C0 03 00 04 */	lfs f0, 4(r3)
/* 8000D654 00009574  C0 82 82 20 */	lfs f4, lbl_802F2A20  //-_SDA2_BASE_(r2)
/* 8000D658 00009578  C0 42 82 24 */	lfs f2, lbl_802F2A24  //-_SDA2_BASE_(r2)
/* 8000D65C 0000957C  C0 23 00 08 */	lfs f1, 8(r3)
/* 8000D660 00009580  EC 60 20 28 */	fsubs f3, f0, f4
/* 8000D664 00009584  C0 02 82 28 */	lfs f0, lbl_802F2A28  //-_SDA2_BASE_(r2)
/* 8000D668 00009588  EC 22 08 28 */	fsubs f1, f2, f1
/* 8000D66C 0000958C  EF E3 20 24 */	fdivs f31, f3, f4
/* 8000D670 00009590  EF C1 00 24 */	fdivs f30, f1, f0
lbl_8000D674:
/* 8000D674 00009594  C0 22 81 B8 */	lfs f1, 60.0f  //lbl_802F29B8  //-_SDA2_BASE_(r2)
/* 8000D678 00009598  38 61 00 54 */	addi r3, r1, 0x54
/* 8000D67C 0000959C  C0 42 82 2C */	lfs f2, lbl_802F2A2C  //-_SDA2_BASE_(r2)
/* 8000D680 000095A0  C0 62 82 30 */	lfs f3, lbl_802F2A30  //-_SDA2_BASE_(r2)
/* 8000D684 000095A4  C0 82 81 80 */	lfs f4, 20000.0f  //lbl_802F2980  //-_SDA2_BASE_(r2)
/* 8000D688 000095A8  48 0B D0 69 */	bl C_MTXPerspective
/* 8000D68C 000095AC  C0 01 00 54 */	lfs f0, 0x54(r1)
/* 8000D690 000095B0  38 61 00 54 */	addi r3, r1, 0x54
/* 8000D694 000095B4  C0 22 82 2C */	lfs f1, lbl_802F2A2C  //-_SDA2_BASE_(r2)
/* 8000D698 000095B8  38 80 00 00 */	li r4, 0
/* 8000D69C 000095BC  EC 00 07 F2 */	fmuls f0, f0, f31  // [0][0] * f31
/* 8000D6A0 000095C0  C0 42 82 34 */	lfs f2, lbl_802F2A34  //-_SDA2_BASE_(r2)
/* 8000D6A4 000095C4  C0 61 00 5C */	lfs f3, 0x5c(r1)
/* 8000D6A8 000095C8  EC 01 00 32 */	fmuls f0, f1, f0  // [0][0] * f31 * 1.3333332538604736f
/* 8000D6AC 000095CC  EC 02 00 32 */	fmuls f0, f2, f0  // [0][0] * f31 * 1.3333332538604736f * 0.5773502588272095f
/* 8000D6B0 000095D0  EC 03 00 28 */	fsubs f0, f3, f0
/* 8000D6B4 000095D4  D0 01 00 5C */	stfs f0, 0x5c(r1)
/* 8000D6B8 000095D8  C0 01 00 68 */	lfs f0, 0x68(r1)
/* 8000D6BC 000095DC  C0 21 00 6C */	lfs f1, 0x6c(r1)
/* 8000D6C0 000095E0  EC 00 07 B2 */	fmuls f0, f0, f30
/* 8000D6C4 000095E4  EC 02 00 32 */	fmuls f0, f2, f0
/* 8000D6C8 000095E8  EC 01 00 28 */	fsubs f0, f1, f0
/* 8000D6CC 000095EC  D0 01 00 6C */	stfs f0, 0x6c(r1)
/* 8000D6D0 000095F0  48 0D 6C D9 */	bl GXSetProjection
/* 8000D6D4 000095F4  3C 60 80 1F */	lis r3, lbl_801F3A58@ha
/* 8000D6D8 000095F8  80 8D 99 24 */	lwz r4, naomiCommonObj  //@sda21(r13)
/* 8000D6DC 000095FC  38 63 3A 58 */	addi r3, r3, lbl_801F3A58@l
/* 8000D6E0 00009600  83 CD 99 6C */	lwz r30, lbl_802F1B4C  //@sda21(r13)
/* 8000D6E4 00009604  A8 A3 00 04 */	lha r5, 4(r3)
/* 8000D6E8 00009608  3C C0 43 30 */	lis r6, 0x4330
/* 8000D6EC 0000960C  A8 03 00 06 */	lha r0, 6(r3)
/* 8000D6F0 00009610  38 7E 00 00 */	addi r3, r30, 0
/* 8000D6F4 00009614  6C A5 80 00 */	xoris r5, r5, 0x8000
/* 8000D6F8 00009618  6C 00 80 00 */	xoris r0, r0, 0x8000
/* 8000D6FC 0000961C  90 A1 00 BC */	stw r5, 0xbc(r1)
/* 8000D700 00009620  80 84 00 AC */	lwz r4, 0xac(r4)
/* 8000D704 00009624  90 01 00 B4 */	stw r0, 0xb4(r1)
/* 8000D708 00009628  80 A4 FF FC */	lwz r5, -4(r4)
/* 8000D70C 0000962C  90 C1 00 B8 */	stw r6, 0xb8(r1)
/* 8000D710 00009630  C8 42 81 70 */	lfd f2, 4503601774854144.0  //lbl_802F2970  //-_SDA2_BASE_(r2)
/* 8000D714 00009634  90 C1 00 B0 */	stw r6, 0xb0(r1)
/* 8000D718 00009638  C8 21 00 B8 */	lfd f1, 0xb8(r1)
/* 8000D71C 0000963C  C8 01 00 B0 */	lfd f0, 0xb0(r1)
/* 8000D720 00009640  EC 21 10 28 */	fsubs f1, f1, f2  // lbl_801F3A58.unk4
/* 8000D724 00009644  80 A5 00 00 */	lwz r5, 0(r5)
/* 8000D728 00009648  EC 00 10 28 */	fsubs f0, f0, f2  // lbl_801F3A58.unk6
/* 8000D72C 0000964C  EF E1 00 24 */	fdivs f31, f1, f0
/* 8000D730 00009650  4B FF 5C E5 */	bl memcpy
/* 8000D734 00009654  C8 02 81 58 */	lfd f0, 0.5  //lbl_802F2958  //-_SDA2_BASE_(r2)
/* 8000D738 00009658  38 7E 00 18 */	addi r3, r30, 0x18
/* 8000D73C 0000965C  C8 22 81 D0 */	lfd f1, 2.0  //lbl_802F29D0  //-_SDA2_BASE_(r2)
/* 8000D740 00009660  FC 1F 00 28 */	fsub f0, f31, f0
/* 8000D744 00009664  C8 62 81 D8 */	lfd f3, 0.0  //lbl_802F29D8  //-_SDA2_BASE_(r2)
/* 8000D748 00009668  81 1E 00 6C */	lwz r8, 0x6c(r30)
/* 8000D74C 0000966C  FC 81 00 32 */	fmul f4, f1, f0
/* 8000D750 00009670  FC 80 20 18 */	frsp f4, f4
/* 8000D754 00009674  FC 04 18 40 */	fcmpo cr0, f4, f3
/* 8000D758 00009678  40 80 00 08 */	bge lbl_8000D760
/* 8000D75C 0000967C  48 00 00 18 */	b lbl_8000D774
lbl_8000D760:
/* 8000D760 00009680  C8 62 81 10 */	lfd f3, 1.0  //lbl_802F2910  //-_SDA2_BASE_(r2)
/* 8000D764 00009684  FC 04 18 40 */	fcmpo cr0, f4, f3
/* 8000D768 00009688  40 81 00 08 */	ble lbl_8000D770
/* 8000D76C 0000968C  48 00 00 08 */	b lbl_8000D774
lbl_8000D770:
/* 8000D770 00009690  FC 60 20 90 */	fmr f3, f4
lbl_8000D774:
/* 8000D774 00009694  6D 00 80 00 */	xoris r0, r8, 0x8000
/* 8000D778 00009698  C8 42 81 70 */	lfd f2, 4503601774854144.0  //lbl_802F2970  //-_SDA2_BASE_(r2)
/* 8000D77C 0000969C  90 01 00 B4 */	stw r0, 0xb4(r1)
/* 8000D780 000096A0  3C 80 43 30 */	lis r4, 0x4330
/* 8000D784 000096A4  C8 02 81 D0 */	lfd f0, 2.0  //lbl_802F29D0  //-_SDA2_BASE_(r2)
/* 8000D788 000096A8  FC 80 18 18 */	frsp f4, f3
/* 8000D78C 000096AC  90 81 00 B0 */	stw r4, 0xb0(r1)
/* 8000D790 000096B0  C8 C2 81 58 */	lfd f6, 0.5  //lbl_802F2958  //-_SDA2_BASE_(r2)
/* 8000D794 000096B4  C8 21 00 B0 */	lfd f1, 0xb0(r1)
/* 8000D798 000096B8  FC 21 10 28 */	fsub f1, f1, f2  // r8
/* 8000D79C 000096BC  FC 01 00 28 */	fsub f0, f1, f0  // r8 - 2.0
/* 8000D7A0 000096C0  FC 64 00 32 */	fmul f3, f4, f0  // f4 * (r8 - 2.0)
/* 8000D7A4 000096C4  FC 60 18 18 */	frsp f3, f3
/* 8000D7A8 000096C8  FC 26 00 F2 */	fmul f1, f6, f3
/* 8000D7AC 000096CC  FC 20 08 18 */	frsp f1, f1  // f3 * 0.5
/* 8000D7B0 000096D0  FC 00 04 8E */	mffs f0
/* 8000D7B4 000096D4  FF C0 00 4C */	mtfsb1 0x1e
/* 8000D7B8 000096D8  FF E0 00 4C */	mtfsb1 0x1f
/* 8000D7BC 000096DC  FC 20 08 1C */	fctiw f1, f1
/* 8000D7C0 000096E0  D8 21 00 10 */	stfd f1, 0x10(r1)
/* 8000D7C4 000096E4  FD FE 05 8E */	mtfsf 0xff, f0
/* 8000D7C8 000096E8  80 01 00 14 */	lwz r0, 0x14(r1)  // mathutil_floor_to_int(f3 * 0.5)
/* 8000D7CC 000096EC  C0 02 82 38 */	lfs f0, lbl_802F2A38  //-_SDA2_BASE_(r2)
/* 8000D7D0 000096F0  38 A3 00 58 */	addi r5, r3, 0x58
/* 8000D7D4 000096F4  6C 00 80 00 */	xoris r0, r0, 0x8000
/* 8000D7D8 000096F8  90 01 00 BC */	stw r0, 0xbc(r1)
/* 8000D7DC 000096FC  38 C8 FF FF */	addi r6, r8, -1
/* 8000D7E0 00009700  90 81 00 B8 */	stw r4, 0xb8(r1)
/* 8000D7E4 00009704  C8 21 00 B8 */	lfd f1, 0xb8(r1)
/* 8000D7E8 00009708  EC 21 10 28 */	fsubs f1, f1, f2  // (float)x
/* 8000D7EC 0000970C  EC 00 00 72 */	fmuls f0, f0, f1  // x * 2.0f
/* 8000D7F0 00009710  FC 00 00 1E */	fctiwz f0, f0
/* 8000D7F4 00009714  D8 01 00 A8 */	stfd f0, 0xa8(r1)
/* 8000D7F8 00009718  80 E1 00 AC */	lwz r7, 0xac(r1)     // (int)(x * 2.0f)
/* 8000D7FC 0000971C  6C E0 80 00 */	xoris r0, r7, 0x8000
/* 8000D800 00009720  90 01 00 A4 */	stw r0, 0xa4(r1)
/* 8000D804 00009724  90 81 00 A0 */	stw r4, 0xa0(r1)
/* 8000D808 00009728  C8 01 00 A0 */	lfd f0, 0xa0(r1)
/* 8000D80C 0000972C  EC 00 10 28 */	fsubs f0, f0, f2  // r7
/* 8000D810 00009730  EC 03 00 28 */	fsubs f0, f3, f0
/* 8000D814 00009734  FC 26 00 32 */	fmul f1, f6, f0   // (r7 - f3) * 0.5
/* 8000D818 00009738  FC 20 08 18 */	frsp f1, f1
/* 8000D81C 0000973C  C8 62 80 D8 */	lfd f3, 0.25  //lbl_802F28D8  //-_SDA2_BASE_(r2)
/* 8000D820 00009740  38 06 00 01 */	addi r0, r6, 1
/* 8000D824 00009744  2C 06 00 00 */	cmpwi r6, 0
/* 8000D828 00009748  7C 09 03 A6 */	mtctr r0
/* 8000D82C 0000974C  FC 03 00 72 */	fmul f0, f3, f1
/* 8000D830 00009750  C0 A2 81 6C */	lfs f5, 0.25f  //lbl_802F296C  //-_SDA2_BASE_(r2)
/* 8000D834 00009754  38 07 00 04 */	addi r0, r7, 4
/* 8000D838 00009758  C0 82 82 3C */	lfs f4, lbl_802F2A3C  //-_SDA2_BASE_(r2)
/* 8000D83C 0000975C  38 87 00 02 */	addi r4, r7, 2
/* 8000D840 00009760  FC 46 00 2A */	fadd f2, f6, f0
/* 8000D844 00009764  FC 03 00 2A */	fadd f0, f3, f0
/* 8000D848 00009768  FC 40 10 18 */	frsp f2, f2
/* 8000D84C 0000976C  FC 00 00 18 */	frsp f0, f0
/* 8000D850 00009770  41 80 00 54 */	blt lbl_8000D8A4
lbl_8000D854:
/* 8000D854 00009774  FC 1F 30 40 */	fcmpo cr0, f31, f6
/* 8000D858 00009778  40 80 00 0C */	bge lbl_8000D864
/* 8000D85C 0000977C  D0 A5 00 18 */	stfs f5, 0x18(r5)
/* 8000D860 00009780  48 00 00 38 */	b lbl_8000D898
lbl_8000D864:
/* 8000D864 00009784  7C 06 38 00 */	cmpw r6, r7
/* 8000D868 00009788  40 80 00 0C */	bge lbl_8000D874
/* 8000D86C 0000978C  D0 85 00 18 */	stfs f4, 0x18(r5)
/* 8000D870 00009790  48 00 00 28 */	b lbl_8000D898
lbl_8000D874:
/* 8000D874 00009794  7C 06 20 00 */	cmpw r6, r4
/* 8000D878 00009798  40 80 00 0C */	bge lbl_8000D884
/* 8000D87C 0000979C  D0 45 00 18 */	stfs f2, 0x18(r5)
/* 8000D880 000097A0  48 00 00 18 */	b lbl_8000D898
lbl_8000D884:
/* 8000D884 000097A4  7C 06 00 00 */	cmpw r6, r0
/* 8000D888 000097A8  40 80 00 0C */	bge lbl_8000D894
/* 8000D88C 000097AC  D0 05 00 18 */	stfs f0, 0x18(r5)
/* 8000D890 000097B0  48 00 00 08 */	b lbl_8000D898
lbl_8000D894:
/* 8000D894 000097B4  D0 A5 00 18 */	stfs f5, 0x18(r5)
lbl_8000D898:
/* 8000D898 000097B8  38 C6 FF FF */	addi r6, r6, -1
/* 8000D89C 000097BC  38 A5 00 20 */	addi r5, r5, 0x20
/* 8000D8A0 000097C0  42 00 FF B4 */	bdnz lbl_8000D854
lbl_8000D8A4:
/* 8000D8A4 000097C4  C8 02 81 58 */	lfd f0, 0.5  //lbl_802F2958  //-_SDA2_BASE_(r2)
/* 8000D8A8 000097C8  FC 1F 00 40 */	fcmpo cr0, f31, f0
/* 8000D8AC 000097CC  4C 41 13 82 */	cror 2, 1, 2
/* 8000D8B0 000097D0  40 82 01 00 */	bne lbl_8000D9B0
/* 8000D8B4 000097D4  C8 42 81 10 */	lfd f2, 1.0  //lbl_802F2910  //-_SDA2_BASE_(r2)
/* 8000D8B8 000097D8  38 08 FF FC */	addi r0, r8, -4
/* 8000D8BC 000097DC  7C 07 00 50 */	subf r0, r7, r0
/* 8000D8C0 000097E0  C8 82 82 40 */	lfd f4, lbl_802F2A40  //-_SDA2_BASE_(r2)
/* 8000D8C4 000097E4  FC 42 08 28 */	fsub f2, f2, f1
/* 8000D8C8 000097E8  54 00 28 34 */	slwi r0, r0, 5
/* 8000D8CC 000097EC  7C A3 02 14 */	add r5, r3, r0
/* 8000D8D0 000097F0  FC 40 10 18 */	frsp f2, f2
/* 8000D8D4 000097F4  C0 A5 00 58 */	lfs f5, 0x58(r5)
/* 8000D8D8 000097F8  C0 65 00 98 */	lfs f3, 0x98(r5)
/* 8000D8DC 000097FC  EC A5 00 72 */	fmuls f5, f5, f1
/* 8000D8E0 00009800  EC 63 00 B2 */	fmuls f3, f3, f2
/* 8000D8E4 00009804  EC 65 18 2A */	fadds f3, f5, f3
/* 8000D8E8 00009808  D0 61 00 48 */	stfs f3, 0x48(r1)
/* 8000D8EC 0000980C  C0 A5 00 5C */	lfs f5, 0x5c(r5)
/* 8000D8F0 00009810  C0 65 00 9C */	lfs f3, 0x9c(r5)
/* 8000D8F4 00009814  EC A5 00 72 */	fmuls f5, f5, f1
/* 8000D8F8 00009818  EC 63 00 B2 */	fmuls f3, f3, f2
/* 8000D8FC 0000981C  EC 65 18 2A */	fadds f3, f5, f3
/* 8000D900 00009820  D0 61 00 4C */	stfs f3, 0x4c(r1)
/* 8000D904 00009824  C0 A5 00 60 */	lfs f5, 0x60(r5)
/* 8000D908 00009828  C0 65 00 A0 */	lfs f3, 0xa0(r5)
/* 8000D90C 0000982C  EC A5 00 72 */	fmuls f5, f5, f1
/* 8000D910 00009830  EC 63 00 B2 */	fmuls f3, f3, f2
/* 8000D914 00009834  EC 65 18 2A */	fadds f3, f5, f3
/* 8000D918 00009838  D0 61 00 50 */	stfs f3, 0x50(r1)
/* 8000D91C 0000983C  C0 A5 00 78 */	lfs f5, 0x78(r5)
/* 8000D920 00009840  C0 65 00 B8 */	lfs f3, 0xb8(r5)
/* 8000D924 00009844  ED 05 00 72 */	fmuls f8, f5, f1
/* 8000D928 00009848  C0 C1 00 48 */	lfs f6, 0x48(r1)
/* 8000D92C 0000984C  EC E3 00 B2 */	fmuls f7, f3, f2
/* 8000D930 00009850  C0 61 00 50 */	lfs f3, 0x50(r1)
/* 8000D934 00009854  C0 A1 00 4C */	lfs f5, 0x4c(r1)
/* 8000D938 00009858  EC E8 38 2A */	fadds f7, f8, f7
/* 8000D93C 0000985C  D0 E1 00 3C */	stfs f7, 0x3c(r1)
/* 8000D940 00009860  C1 05 00 7C */	lfs f8, 0x7c(r5)
/* 8000D944 00009864  C0 E5 00 BC */	lfs f7, 0xbc(r5)
/* 8000D948 00009868  ED 08 00 72 */	fmuls f8, f8, f1
/* 8000D94C 0000986C  EC E7 00 B2 */	fmuls f7, f7, f2
/* 8000D950 00009870  EC E8 38 2A */	fadds f7, f8, f7
/* 8000D954 00009874  D0 E1 00 40 */	stfs f7, 0x40(r1)
/* 8000D958 00009878  C1 05 00 80 */	lfs f8, 0x80(r5)
/* 8000D95C 0000987C  C0 E5 00 C0 */	lfs f7, 0xc0(r5)
/* 8000D960 00009880  ED 08 00 72 */	fmuls f8, f8, f1
/* 8000D964 00009884  EC 27 00 B2 */	fmuls f1, f7, f2
/* 8000D968 00009888  EC 28 08 2A */	fadds f1, f8, f1
/* 8000D96C 0000988C  D0 21 00 44 */	stfs f1, 0x44(r1)
/* 8000D970 00009890  C0 41 00 3C */	lfs f2, 0x3c(r1)
/* 8000D974 00009894  C0 21 00 44 */	lfs f1, 0x44(r1)
/* 8000D978 00009898  EC C6 10 2A */	fadds f6, f6, f2
/* 8000D97C 0000989C  C0 41 00 40 */	lfs f2, 0x40(r1)
/* 8000D980 000098A0  EC 23 08 2A */	fadds f1, f3, f1
/* 8000D984 000098A4  EC 45 10 2A */	fadds f2, f5, f2
/* 8000D988 000098A8  FC 60 01 B2 */	fmul f3, f0, f6
/* 8000D98C 000098AC  FC 20 00 72 */	fmul f1, f0, f1
/* 8000D990 000098B0  FC 40 00 B2 */	fmul f2, f0, f2
/* 8000D994 000098B4  FC 04 08 2A */	fadd f0, f4, f1
/* 8000D998 000098B8  FC 60 18 18 */	frsp f3, f3
/* 8000D99C 000098BC  FC 20 10 18 */	frsp f1, f2
/* 8000D9A0 000098C0  FC 00 00 18 */	frsp f0, f0
/* 8000D9A4 000098C4  D0 61 00 94 */	stfs f3, 0x94(r1)
/* 8000D9A8 000098C8  D0 21 00 98 */	stfs f1, 0x98(r1)
/* 8000D9AC 000098CC  D0 01 00 9C */	stfs f0, 0x9c(r1)
lbl_8000D9B0:
/* 8000D9B0 000098D0  C8 02 81 D0 */	lfd f0, 2.0  //lbl_802F29D0  //-_SDA2_BASE_(r2)
/* 8000D9B4 000098D4  80 03 00 4C */	lwz r0, 0x4c(r3)
/* 8000D9B8 000098D8  FC 80 07 F2 */	fmul f4, f0, f31
/* 8000D9BC 000098DC  C8 22 81 D8 */	lfd f1, 0.0  //lbl_802F29D8  //-_SDA2_BASE_(r2)
/* 8000D9C0 000098E0  7C 04 16 70 */	srawi r4, r0, 2
/* 8000D9C4 000098E4  38 04 00 14 */	addi r0, r4, 0x14
/* 8000D9C8 000098E8  54 00 10 3A */	slwi r0, r0, 2
/* 8000D9CC 000098EC  FC 80 20 18 */	frsp f4, f4
/* 8000D9D0 000098F0  7C 63 02 14 */	add r3, r3, r0
/* 8000D9D4 000098F4  81 03 00 54 */	lwz r8, 0x54(r3)
/* 8000D9D8 000098F8  FC 04 08 40 */	fcmpo cr0, f4, f1
/* 8000D9DC 000098FC  40 80 00 08 */	bge lbl_8000D9E4
/* 8000D9E0 00009900  48 00 00 18 */	b lbl_8000D9F8
lbl_8000D9E4:
/* 8000D9E4 00009904  C8 22 81 10 */	lfd f1, 1.0  //lbl_802F2910  //-_SDA2_BASE_(r2)
/* 8000D9E8 00009908  FC 04 08 40 */	fcmpo cr0, f4, f1
/* 8000D9EC 0000990C  40 81 00 08 */	ble lbl_8000D9F4
/* 8000D9F0 00009910  48 00 00 08 */	b lbl_8000D9F8
lbl_8000D9F4:
/* 8000D9F4 00009914  FC 20 20 90 */	fmr f1, f4
lbl_8000D9F8:
/* 8000D9F8 00009918  6D 00 80 00 */	xoris r0, r8, 0x8000
/* 8000D9FC 0000991C  C8 42 81 70 */	lfd f2, 4503601774854144.0  //lbl_802F2970  //-_SDA2_BASE_(r2)
/* 8000DA00 00009920  90 01 00 A4 */	stw r0, 0xa4(r1)
/* 8000DA04 00009924  3C 80 43 30 */	lis r4, 0x4330
/* 8000DA08 00009928  C8 02 81 D0 */	lfd f0, 2.0  //lbl_802F29D0  //-_SDA2_BASE_(r2)
/* 8000DA0C 0000992C  FC 80 08 18 */	frsp f4, f1
/* 8000DA10 00009930  90 81 00 A0 */	stw r4, 0xa0(r1)
/* 8000DA14 00009934  C8 C2 81 58 */	lfd f6, 0.5  //lbl_802F2958  //-_SDA2_BASE_(r2)
/* 8000DA18 00009938  C8 21 00 A0 */	lfd f1, 0xa0(r1)
/* 8000DA1C 0000993C  FC 21 10 28 */	fsub f1, f1, f2
/* 8000DA20 00009940  FC 01 00 28 */	fsub f0, f1, f0
/* 8000DA24 00009944  FC 64 00 32 */	fmul f3, f4, f0
/* 8000DA28 00009948  FC 60 18 18 */	frsp f3, f3
/* 8000DA2C 0000994C  FC 26 00 F2 */	fmul f1, f6, f3
/* 8000DA30 00009950  FC 20 08 18 */	frsp f1, f1
/* 8000DA34 00009954  FC 00 04 8E */	mffs f0
/* 8000DA38 00009958  FF C0 00 4C */	mtfsb1 0x1e
/* 8000DA3C 0000995C  FF E0 00 4C */	mtfsb1 0x1f
/* 8000DA40 00009960  FC 20 08 1C */	fctiw f1, f1
/* 8000DA44 00009964  D8 21 00 08 */	stfd f1, 8(r1)
/* 8000DA48 00009968  FD FE 05 8E */	mtfsf 0xff, f0
/* 8000DA4C 0000996C  80 01 00 0C */	lwz r0, 0xc(r1)
/* 8000DA50 00009970  C0 02 82 38 */	lfs f0, lbl_802F2A38  //-_SDA2_BASE_(r2)
/* 8000DA54 00009974  38 A3 00 58 */	addi r5, r3, 0x58
/* 8000DA58 00009978  6C 00 80 00 */	xoris r0, r0, 0x8000
/* 8000DA5C 0000997C  90 01 00 AC */	stw r0, 0xac(r1)
/* 8000DA60 00009980  38 C8 FF FF */	addi r6, r8, -1
/* 8000DA64 00009984  90 81 00 A8 */	stw r4, 0xa8(r1)
/* 8000DA68 00009988  C8 21 00 A8 */	lfd f1, 0xa8(r1)
/* 8000DA6C 0000998C  EC 21 10 28 */	fsubs f1, f1, f2
/* 8000DA70 00009990  EC 00 00 72 */	fmuls f0, f0, f1
/* 8000DA74 00009994  FC 00 00 1E */	fctiwz f0, f0
/* 8000DA78 00009998  D8 01 00 B0 */	stfd f0, 0xb0(r1)
/* 8000DA7C 0000999C  80 E1 00 B4 */	lwz r7, 0xb4(r1)
/* 8000DA80 000099A0  6C E0 80 00 */	xoris r0, r7, 0x8000
/* 8000DA84 000099A4  90 01 00 BC */	stw r0, 0xbc(r1)
/* 8000DA88 000099A8  90 81 00 B8 */	stw r4, 0xb8(r1)
/* 8000DA8C 000099AC  C8 01 00 B8 */	lfd f0, 0xb8(r1)
/* 8000DA90 000099B0  EC 00 10 28 */	fsubs f0, f0, f2
/* 8000DA94 000099B4  EC 03 00 28 */	fsubs f0, f3, f0
/* 8000DA98 000099B8  FC 26 00 32 */	fmul f1, f6, f0
/* 8000DA9C 000099BC  FC 20 08 18 */	frsp f1, f1
/* 8000DAA0 000099C0  C8 82 80 D8 */	lfd f4, 0.25  //lbl_802F28D8  //-_SDA2_BASE_(r2)
/* 8000DAA4 000099C4  38 06 00 01 */	addi r0, r6, 1
/* 8000DAA8 000099C8  2C 06 00 00 */	cmpwi r6, 0
/* 8000DAAC 000099CC  7C 09 03 A6 */	mtctr r0
/* 8000DAB0 000099D0  FC 44 00 72 */	fmul f2, f4, f1
/* 8000DAB4 000099D4  C0 A2 82 3C */	lfs f5, lbl_802F2A3C  //-_SDA2_BASE_(r2)
/* 8000DAB8 000099D8  38 07 00 04 */	addi r0, r7, 4
/* 8000DABC 000099DC  C0 02 81 6C */	lfs f0, 0.25f  //lbl_802F296C  //-_SDA2_BASE_(r2)
/* 8000DAC0 000099E0  38 87 00 02 */	addi r4, r7, 2
/* 8000DAC4 000099E4  FC 66 10 2A */	fadd f3, f6, f2
/* 8000DAC8 000099E8  FC 44 10 2A */	fadd f2, f4, f2
/* 8000DACC 000099EC  FC 60 18 18 */	frsp f3, f3
/* 8000DAD0 000099F0  FC 40 10 18 */	frsp f2, f2
/* 8000DAD4 000099F4  41 80 00 54 */	blt lbl_8000DB28
lbl_8000DAD8:
/* 8000DAD8 000099F8  FC 1F 30 40 */	fcmpo cr0, f31, f6
/* 8000DADC 000099FC  40 81 00 0C */	ble lbl_8000DAE8
/* 8000DAE0 00009A00  D0 A5 00 18 */	stfs f5, 0x18(r5)
/* 8000DAE4 00009A04  48 00 00 38 */	b lbl_8000DB1C
lbl_8000DAE8:
/* 8000DAE8 00009A08  7C 06 38 00 */	cmpw r6, r7
/* 8000DAEC 00009A0C  40 80 00 0C */	bge lbl_8000DAF8
/* 8000DAF0 00009A10  D0 A5 00 18 */	stfs f5, 0x18(r5)
/* 8000DAF4 00009A14  48 00 00 28 */	b lbl_8000DB1C
lbl_8000DAF8:
/* 8000DAF8 00009A18  7C 06 20 00 */	cmpw r6, r4
/* 8000DAFC 00009A1C  40 80 00 0C */	bge lbl_8000DB08
/* 8000DB00 00009A20  D0 65 00 18 */	stfs f3, 0x18(r5)
/* 8000DB04 00009A24  48 00 00 18 */	b lbl_8000DB1C
lbl_8000DB08:
/* 8000DB08 00009A28  7C 06 00 00 */	cmpw r6, r0
/* 8000DB0C 00009A2C  40 80 00 0C */	bge lbl_8000DB18
/* 8000DB10 00009A30  D0 45 00 18 */	stfs f2, 0x18(r5)
/* 8000DB14 00009A34  48 00 00 08 */	b lbl_8000DB1C
lbl_8000DB18:
/* 8000DB18 00009A38  D0 05 00 18 */	stfs f0, 0x18(r5)
lbl_8000DB1C:
/* 8000DB1C 00009A3C  38 C6 FF FF */	addi r6, r6, -1
/* 8000DB20 00009A40  38 A5 00 20 */	addi r5, r5, 0x20
/* 8000DB24 00009A44  42 00 FF B4 */	bdnz lbl_8000DAD8
lbl_8000DB28:
/* 8000DB28 00009A48  C8 02 81 58 */	lfd f0, 0.5  //lbl_802F2958  //-_SDA2_BASE_(r2)
/* 8000DB2C 00009A4C  FC 1F 00 40 */	fcmpo cr0, f31, f0
/* 8000DB30 00009A50  40 80 01 00 */	bge lbl_8000DC30
/* 8000DB34 00009A54  C8 42 81 10 */	lfd f2, 1.0  //lbl_802F2910  //-_SDA2_BASE_(r2)
/* 8000DB38 00009A58  38 08 FF FC */	addi r0, r8, -4
/* 8000DB3C 00009A5C  7C 07 00 50 */	subf r0, r7, r0
/* 8000DB40 00009A60  C8 82 82 40 */	lfd f4, lbl_802F2A40  //-_SDA2_BASE_(r2)
/* 8000DB44 00009A64  FC 42 08 28 */	fsub f2, f2, f1
/* 8000DB48 00009A68  54 00 28 34 */	slwi r0, r0, 5
/* 8000DB4C 00009A6C  7C A3 02 14 */	add r5, r3, r0
/* 8000DB50 00009A70  FC 40 10 18 */	frsp f2, f2
/* 8000DB54 00009A74  C0 A5 00 58 */	lfs f5, 0x58(r5)
/* 8000DB58 00009A78  C0 65 00 98 */	lfs f3, 0x98(r5)
/* 8000DB5C 00009A7C  EC A5 00 72 */	fmuls f5, f5, f1
/* 8000DB60 00009A80  EC 63 00 B2 */	fmuls f3, f3, f2
/* 8000DB64 00009A84  EC 65 18 2A */	fadds f3, f5, f3
/* 8000DB68 00009A88  D0 61 00 30 */	stfs f3, 0x30(r1)
/* 8000DB6C 00009A8C  C0 A5 00 5C */	lfs f5, 0x5c(r5)
/* 8000DB70 00009A90  C0 65 00 9C */	lfs f3, 0x9c(r5)
/* 8000DB74 00009A94  EC A5 00 72 */	fmuls f5, f5, f1
/* 8000DB78 00009A98  EC 63 00 B2 */	fmuls f3, f3, f2
/* 8000DB7C 00009A9C  EC 65 18 2A */	fadds f3, f5, f3
/* 8000DB80 00009AA0  D0 61 00 34 */	stfs f3, 0x34(r1)
/* 8000DB84 00009AA4  C0 A5 00 60 */	lfs f5, 0x60(r5)
/* 8000DB88 00009AA8  C0 65 00 A0 */	lfs f3, 0xa0(r5)
/* 8000DB8C 00009AAC  EC A5 00 72 */	fmuls f5, f5, f1
/* 8000DB90 00009AB0  EC 63 00 B2 */	fmuls f3, f3, f2
/* 8000DB94 00009AB4  EC 65 18 2A */	fadds f3, f5, f3
/* 8000DB98 00009AB8  D0 61 00 38 */	stfs f3, 0x38(r1)
/* 8000DB9C 00009ABC  C0 A5 00 78 */	lfs f5, 0x78(r5)
/* 8000DBA0 00009AC0  C0 65 00 B8 */	lfs f3, 0xb8(r5)
/* 8000DBA4 00009AC4  ED 05 00 72 */	fmuls f8, f5, f1
/* 8000DBA8 00009AC8  C0 C1 00 30 */	lfs f6, 0x30(r1)
/* 8000DBAC 00009ACC  EC E3 00 B2 */	fmuls f7, f3, f2
/* 8000DBB0 00009AD0  C0 61 00 38 */	lfs f3, 0x38(r1)
/* 8000DBB4 00009AD4  C0 A1 00 34 */	lfs f5, 0x34(r1)
/* 8000DBB8 00009AD8  EC E8 38 2A */	fadds f7, f8, f7
/* 8000DBBC 00009ADC  D0 E1 00 24 */	stfs f7, 0x24(r1)
/* 8000DBC0 00009AE0  C1 05 00 7C */	lfs f8, 0x7c(r5)
/* 8000DBC4 00009AE4  C0 E5 00 BC */	lfs f7, 0xbc(r5)
/* 8000DBC8 00009AE8  ED 08 00 72 */	fmuls f8, f8, f1
/* 8000DBCC 00009AEC  EC E7 00 B2 */	fmuls f7, f7, f2
/* 8000DBD0 00009AF0  EC E8 38 2A */	fadds f7, f8, f7
/* 8000DBD4 00009AF4  D0 E1 00 28 */	stfs f7, 0x28(r1)
/* 8000DBD8 00009AF8  C1 05 00 80 */	lfs f8, 0x80(r5)
/* 8000DBDC 00009AFC  C0 E5 00 C0 */	lfs f7, 0xc0(r5)
/* 8000DBE0 00009B00  ED 08 00 72 */	fmuls f8, f8, f1
/* 8000DBE4 00009B04  EC 27 00 B2 */	fmuls f1, f7, f2
/* 8000DBE8 00009B08  EC 28 08 2A */	fadds f1, f8, f1
/* 8000DBEC 00009B0C  D0 21 00 2C */	stfs f1, 0x2c(r1)
/* 8000DBF0 00009B10  C0 41 00 24 */	lfs f2, 0x24(r1)
/* 8000DBF4 00009B14  C0 21 00 2C */	lfs f1, 0x2c(r1)
/* 8000DBF8 00009B18  EC C6 10 2A */	fadds f6, f6, f2
/* 8000DBFC 00009B1C  C0 41 00 28 */	lfs f2, 0x28(r1)
/* 8000DC00 00009B20  EC 23 08 2A */	fadds f1, f3, f1
/* 8000DC04 00009B24  EC 45 10 2A */	fadds f2, f5, f2
/* 8000DC08 00009B28  FC 60 01 B2 */	fmul f3, f0, f6
/* 8000DC0C 00009B2C  FC 20 00 72 */	fmul f1, f0, f1
/* 8000DC10 00009B30  FC 40 00 B2 */	fmul f2, f0, f2
/* 8000DC14 00009B34  FC 04 08 2A */	fadd f0, f4, f1
/* 8000DC18 00009B38  FC 60 18 18 */	frsp f3, f3
/* 8000DC1C 00009B3C  FC 20 10 18 */	frsp f1, f2
/* 8000DC20 00009B40  FC 00 00 18 */	frsp f0, f0
/* 8000DC24 00009B44  D0 61 00 94 */	stfs f3, 0x94(r1)
/* 8000DC28 00009B48  D0 21 00 98 */	stfs f1, 0x98(r1)
/* 8000DC2C 00009B4C  D0 01 00 9C */	stfs f0, 0x9c(r1)
lbl_8000DC30:
/* 8000DC30 00009B50  80 1F 00 5C */	lwz r0, 0x5c(r31)
/* 8000DC34 00009B54  2C 00 00 02 */	cmpwi r0, 2
/* 8000DC38 00009B58  41 82 00 B0 */	beq lbl_8000DCE8
/* 8000DC3C 00009B5C  40 80 00 14 */	bge lbl_8000DC50
/* 8000DC40 00009B60  2C 00 00 00 */	cmpwi r0, 0
/* 8000DC44 00009B64  41 82 00 1C */	beq lbl_8000DC60
/* 8000DC48 00009B68  40 80 00 50 */	bge lbl_8000DC98
/* 8000DC4C 00009B6C  48 00 00 CC */	b lbl_8000DD18
lbl_8000DC50:
/* 8000DC50 00009B70  2C 00 00 04 */	cmpwi r0, 4
/* 8000DC54 00009B74  41 82 00 BC */	beq lbl_8000DD10
/* 8000DC58 00009B78  40 80 00 C0 */	bge lbl_8000DD18
/* 8000DC5C 00009B7C  48 00 00 A8 */	b lbl_8000DD04
lbl_8000DC60:
/* 8000DC60 00009B80  3C 60 80 1F */	lis r3, lbl_801F3A58@ha
/* 8000DC64 00009B84  80 03 3A 58 */	lwz r0, lbl_801F3A58@l(r3)
/* 8000DC68 00009B88  54 00 07 39 */	rlwinm. r0, r0, 0, 0x1c, 0x1c
/* 8000DC6C 00009B8C  40 82 00 AC */	bne lbl_8000DD18
/* 8000DC70 00009B90  38 00 00 01 */	li r0, 1
/* 8000DC74 00009B94  90 1F 00 5C */	stw r0, 0x5c(r31)
/* 8000DC78 00009B98  C0 02 82 48 */	lfs f0, lbl_802F2A48  //-_SDA2_BASE_(r2)
/* 8000DC7C 00009B9C  D0 1F 00 70 */	stfs f0, 0x70(r31)
/* 8000DC80 00009BA0  48 0F 8B E5 */	bl rand
/* 8000DC84 00009BA4  54 63 05 BE */	clrlwi r3, r3, 0x16
/* 8000DC88 00009BA8  38 03 04 00 */	addi r0, r3, 0x400
/* 8000DC8C 00009BAC  7C 00 00 D0 */	neg r0, r0
/* 8000DC90 00009BB0  90 1F 00 68 */	stw r0, 0x68(r31)
/* 8000DC94 00009BB4  48 00 00 84 */	b lbl_8000DD18
lbl_8000DC98:
/* 8000DC98 00009BB8  C0 3F 00 70 */	lfs f1, 0x70(r31)
/* 8000DC9C 00009BBC  C8 02 82 50 */	lfd f0, lbl_802F2A50  //-_SDA2_BASE_(r2)
/* 8000DCA0 00009BC0  FC 01 00 28 */	fsub f0, f1, f0
/* 8000DCA4 00009BC4  FC 00 00 18 */	frsp f0, f0
/* 8000DCA8 00009BC8  D0 1F 00 70 */	stfs f0, 0x70(r31)
/* 8000DCAC 00009BCC  C0 3F 00 6C */	lfs f1, 0x6c(r31)
/* 8000DCB0 00009BD0  C0 1F 00 70 */	lfs f0, 0x70(r31)
/* 8000DCB4 00009BD4  EC 01 00 2A */	fadds f0, f1, f0
/* 8000DCB8 00009BD8  D0 1F 00 6C */	stfs f0, 0x6c(r31)
/* 8000DCBC 00009BDC  C0 3F 00 6C */	lfs f1, 0x6c(r31)
/* 8000DCC0 00009BE0  C8 02 81 10 */	lfd f0, 1.0  //lbl_802F2910  //-_SDA2_BASE_(r2)
/* 8000DCC4 00009BE4  FC 01 00 40 */	fcmpo cr0, f1, f0
/* 8000DCC8 00009BE8  40 80 00 50 */	bge lbl_8000DD18
/* 8000DCCC 00009BEC  C0 02 80 D4 */	lfs f0, 1.0f  //lbl_802F28D4  //-_SDA2_BASE_(r2)
/* 8000DCD0 00009BF0  38 00 00 02 */	li r0, 2
/* 8000DCD4 00009BF4  D0 1F 00 6C */	stfs f0, 0x6c(r31)
/* 8000DCD8 00009BF8  C0 02 80 E4 */	lfs f0, 0.0f  //lbl_802F28E4  //-_SDA2_BASE_(r2)
/* 8000DCDC 00009BFC  D0 1F 00 70 */	stfs f0, 0x70(r31)
/* 8000DCE0 00009C00  90 1F 00 5C */	stw r0, 0x5c(r31)
/* 8000DCE4 00009C04  48 00 00 34 */	b lbl_8000DD18
lbl_8000DCE8:
/* 8000DCE8 00009C08  3C 60 80 1F */	lis r3, lbl_801F3A58@ha
/* 8000DCEC 00009C0C  80 03 3A 58 */	lwz r0, lbl_801F3A58@l(r3)
/* 8000DCF0 00009C10  54 00 07 39 */	rlwinm. r0, r0, 0, 0x1c, 0x1c
/* 8000DCF4 00009C14  41 82 00 24 */	beq lbl_8000DD18
/* 8000DCF8 00009C18  38 00 00 03 */	li r0, 3
/* 8000DCFC 00009C1C  90 1F 00 5C */	stw r0, 0x5c(r31)
/* 8000DD00 00009C20  48 00 00 18 */	b lbl_8000DD18
lbl_8000DD04:
/* 8000DD04 00009C24  38 00 00 04 */	li r0, 4
/* 8000DD08 00009C28  90 1F 00 5C */	stw r0, 0x5c(r31)
/* 8000DD0C 00009C2C  48 00 00 0C */	b lbl_8000DD18
lbl_8000DD10:
/* 8000DD10 00009C30  38 00 00 00 */	li r0, 0
/* 8000DD14 00009C34  90 1F 00 5C */	stw r0, 0x5c(r31)
lbl_8000DD18:
/* 8000DD18 00009C38  3C 60 80 1F */	lis r3, lbl_801F3A58@ha
/* 8000DD1C 00009C3C  80 03 3A 58 */	lwz r0, lbl_801F3A58@l(r3)
/* 8000DD20 00009C40  54 00 07 39 */	rlwinm. r0, r0, 0, 0x1c, 0x1c
/* 8000DD24 00009C44  41 82 00 18 */	beq lbl_8000DD3C
/* 8000DD28 00009C48  80 7F 00 68 */	lwz r3, 0x68(r31)
/* 8000DD2C 00009C4C  7C 60 1E 70 */	srawi r0, r3, 3
/* 8000DD30 00009C50  7C 00 18 50 */	subf r0, r0, r3
/* 8000DD34 00009C54  90 1F 00 68 */	stw r0, 0x68(r31)
/* 8000DD38 00009C58  48 00 00 3C */	b lbl_8000DD74
lbl_8000DD3C:
/* 8000DD3C 00009C5C  C8 02 81 58 */	lfd f0, 0.5  //lbl_802F2958  //-_SDA2_BASE_(r2)
/* 8000DD40 00009C60  FC 1F 00 40 */	fcmpo cr0, f31, f0
/* 8000DD44 00009C64  40 81 00 1C */	ble lbl_8000DD60
/* 8000DD48 00009C68  80 7F 00 68 */	lwz r3, 0x68(r31)
/* 8000DD4C 00009C6C  20 03 FD 00 */	subfic r0, r3, -768
/* 8000DD50 00009C70  7C 00 26 70 */	srawi r0, r0, 4
/* 8000DD54 00009C74  7C 03 02 14 */	add r0, r3, r0
/* 8000DD58 00009C78  90 1F 00 68 */	stw r0, 0x68(r31)
/* 8000DD5C 00009C7C  48 00 00 18 */	b lbl_8000DD74
lbl_8000DD60:
/* 8000DD60 00009C80  80 7F 00 68 */	lwz r3, 0x68(r31)
/* 8000DD64 00009C84  20 03 FA 00 */	subfic r0, r3, -1536
/* 8000DD68 00009C88  7C 00 26 70 */	srawi r0, r0, 4
/* 8000DD6C 00009C8C  7C 03 02 14 */	add r0, r3, r0
/* 8000DD70 00009C90  90 1F 00 68 */	stw r0, 0x68(r31)
lbl_8000DD74:
/* 8000DD74 00009C94  80 0D 9D 00 */	lwz r0, gamePauseStatus  //@sda21(r13)
/* 8000DD78 00009C98  70 00 00 0A */	andi. r0, r0, 0xa
/* 8000DD7C 00009C9C  40 82 00 14 */	bne lbl_8000DD90
/* 8000DD80 00009CA0  80 7F 00 64 */	lwz r3, 0x64(r31)
/* 8000DD84 00009CA4  80 1F 00 68 */	lwz r0, 0x68(r31)
/* 8000DD88 00009CA8  7C 03 02 14 */	add r0, r3, r0
/* 8000DD8C 00009CAC  90 1F 00 64 */	stw r0, 0x64(r31)
lbl_8000DD90:
/* 8000DD90 00009CB0  C0 22 80 D4 */	lfs f1, 1.0f  //lbl_802F28D4  //-_SDA2_BASE_(r2)
/* 8000DD94 00009CB4  FC 40 08 90 */	fmr f2, f1
/* 8000DD98 00009CB8  FC 60 08 90 */	fmr f3, f1
/* 8000DD9C 00009CBC  48 02 2E 1D */	bl func_80030BB8
/* 8000DDA0 00009CC0  C0 22 80 D4 */	lfs f1, 1.0f  //lbl_802F28D4  //-_SDA2_BASE_(r2)
/* 8000DDA4 00009CC4  FC 40 F8 90 */	fmr f2, f31
/* 8000DDA8 00009CC8  C0 62 80 E4 */	lfs f3, 0.0f  //lbl_802F28E4  //-_SDA2_BASE_(r2)
/* 8000DDAC 00009CCC  FC 80 08 90 */	fmr f4, f1
/* 8000DDB0 00009CD0  48 08 19 65 */	bl g_avdisp_set_some_color_1
/* 8000DDB4 00009CD4  C8 02 81 10 */	lfd f0, 1.0  //lbl_802F2910  //-_SDA2_BASE_(r2)
/* 8000DDB8 00009CD8  C0 22 80 E4 */	lfs f1, 0.0f  //lbl_802F28E4  //-_SDA2_BASE_(r2)
/* 8000DDBC 00009CDC  FC 40 F8 28 */	fsub f2, f0, f31
/* 8000DDC0 00009CE0  C8 02 81 58 */	lfd f0, 0.5  //lbl_802F2958  //-_SDA2_BASE_(r2)
/* 8000DDC4 00009CE4  FC 60 08 90 */	fmr f3, f1
/* 8000DDC8 00009CE8  FC 42 00 28 */	fsub f2, f2, f0
/* 8000DDCC 00009CEC  FC 40 10 18 */	frsp f2, f2
/* 8000DDD0 00009CF0  4B FF 98 05 */	bl mathutil_mtxA_from_translate_xyz
/* 8000DDD4 00009CF4  38 60 00 00 */	li r3, 0
/* 8000DDD8 00009CF8  80 8D 99 80 */	lwz r4, mathutilData  //@sda21(r13)
/* 8000DDDC 00009CFC  48 08 19 09 */	bl g_avdisp_set_some_matrix
/* 8000DDE0 00009D00  4B FF 97 85 */	bl mathutil_mtxA_from_identity
/* 8000DDE4 00009D04  C0 22 82 58 */	lfs f1, lbl_802F2A58  //-_SDA2_BASE_(r2)
/* 8000DDE8 00009D08  C0 42 82 5C */	lfs f2, lbl_802F2A5C  //-_SDA2_BASE_(r2)
/* 8000DDEC 00009D0C  C0 62 82 60 */	lfs f3, lbl_802F2A60  //-_SDA2_BASE_(r2)
/* 8000DDF0 00009D10  4B FF 9F 41 */	bl mathutil_mtxA_translate_xyz
/* 8000DDF4 00009D14  C3 C2 82 64 */	lfs f30, lbl_802F2A64  //-_SDA2_BASE_(r2)
/* 8000DDF8 00009D18  FC 20 F0 90 */	fmr f1, f30
/* 8000DDFC 00009D1C  4B FF 9F CD */	bl mathutil_mtxA_scale_s
/* 8000DE00 00009D20  80 6D 99 80 */	lwz r3, mathutilData  //@sda21(r13)
/* 8000DE04 00009D24  38 80 00 00 */	li r4, 0
/* 8000DE08 00009D28  48 08 CC 1D */	bl g_gxutil_upload_some_mtx
/* 8000DE0C 00009D2C  FC 20 F0 90 */	fmr f1, f30
/* 8000DE10 00009D30  48 08 06 11 */	bl g_avdisp_set_model_scale
/* 8000DE14 00009D34  38 60 00 01 */	li r3, 1
/* 8000DE18 00009D38  48 08 18 BD */	bl func_8008F6D4
/* 8000DE1C 00009D3C  80 6D 9A E8 */	lwz r3, commonGma  //@sda21(r13)
/* 8000DE20 00009D40  80 63 00 08 */	lwz r3, 8(r3)
/* 8000DE24 00009D44  80 63 02 80 */	lwz r3, 0x280(r3)
/* 8000DE28 00009D48  48 08 09 85 */	bl g_avdisp_draw_model_1
/* 8000DE2C 00009D4C  38 60 00 00 */	li r3, 0
/* 8000DE30 00009D50  48 08 18 A5 */	bl func_8008F6D4
/* 8000DE34 00009D54  C8 02 81 10 */	lfd f0, 1.0  //lbl_802F2910  //-_SDA2_BASE_(r2)
/* 8000DE38 00009D58  3C 60 80 17 */	lis r3, lbl_80173FE0@ha
/* 8000DE3C 00009D5C  38 83 3F E0 */	addi r4, r3, lbl_80173FE0@l
/* 8000DE40 00009D60  C8 22 82 68 */	lfd f1, lbl_802F2A68  //-_SDA2_BASE_(r2)
/* 8000DE44 00009D64  FC 00 F8 28 */	fsub f0, f0, f31
/* 8000DE48 00009D68  38 60 00 0A */	li r3, 0xa
/* 8000DE4C 00009D6C  FC 21 00 32 */	fmul f1, f1, f0
/* 8000DE50 00009D70  FC 20 08 18 */	frsp f1, f1
/* 8000DE54 00009D74  48 03 5A C5 */	bl g_interp_stage_anim_probably
/* 8000DE58 00009D78  C8 02 81 10 */	lfd f0, 1.0  //lbl_802F2910  //-_SDA2_BASE_(r2)
/* 8000DE5C 00009D7C  3C 60 80 17 */	lis r3, lbl_801740A8@ha
/* 8000DE60 00009D80  38 83 40 A8 */	addi r4, r3, lbl_801740A8@l
/* 8000DE64 00009D84  C8 42 82 68 */	lfd f2, lbl_802F2A68  //-_SDA2_BASE_(r2)
/* 8000DE68 00009D88  FC 00 F8 28 */	fsub f0, f0, f31
/* 8000DE6C 00009D8C  D0 21 00 18 */	stfs f1, 0x18(r1)
/* 8000DE70 00009D90  38 60 00 0D */	li r3, 0xd
/* 8000DE74 00009D94  FC 22 00 32 */	fmul f1, f2, f0
/* 8000DE78 00009D98  FC 20 08 18 */	frsp f1, f1
/* 8000DE7C 00009D9C  48 03 5A 9D */	bl g_interp_stage_anim_probably
/* 8000DE80 00009DA0  D0 21 00 1C */	stfs f1, 0x1c(r1)
/* 8000DE84 00009DA4  38 61 00 18 */	addi r3, r1, 0x18
/* 8000DE88 00009DA8  C0 02 82 70 */	lfs f0, lbl_802F2A70  //-_SDA2_BASE_(r2)
/* 8000DE8C 00009DAC  D0 01 00 20 */	stfs f0, 0x20(r1)
/* 8000DE90 00009DB0  4B FF 9E 91 */	bl mathutil_mtxA_translate
/* 8000DE94 00009DB4  4B FF 96 FD */	bl mathutil_mtxA_sq_from_identity
/* 8000DE98 00009DB8  80 7F 00 64 */	lwz r3, 0x64(r31)
/* 8000DE9C 00009DBC  4B FF A1 ED */	bl mathutil_mtxA_rotate_z
/* 8000DEA0 00009DC0  C0 22 82 74 */	lfs f1, lbl_802F2A74  //-_SDA2_BASE_(r2)
/* 8000DEA4 00009DC4  4B FF 9F 25 */	bl mathutil_mtxA_scale_s
/* 8000DEA8 00009DC8  C0 3F 00 6C */	lfs f1, 0x6c(r31)
/* 8000DEAC 00009DCC  FC 40 08 90 */	fmr f2, f1
/* 8000DEB0 00009DD0  FC 60 08 90 */	fmr f3, f1
/* 8000DEB4 00009DD4  4B FF 9F 25 */	bl mathutil_mtxA_scale_xyz
/* 8000DEB8 00009DD8  80 6D 99 24 */	lwz r3, naomiCommonObj  //@sda21(r13)
/* 8000DEBC 00009DDC  80 63 00 A4 */	lwz r3, 0xa4(r3)
/* 8000DEC0 00009DE0  48 02 34 91 */	bl g_draw_naomi_model_1
/* 8000DEC4 00009DE4  48 00 04 F9 */	bl func_8000E3BC
lbl_8000DEC8:
/* 8000DEC8 00009DE8  80 01 00 DC */	lwz r0, 0xdc(r1)
/* 8000DECC 00009DEC  CB E1 00 D0 */	lfd f31, 0xd0(r1)
/* 8000DED0 00009DF0  CB C1 00 C8 */	lfd f30, 0xc8(r1)
/* 8000DED4 00009DF4  7C 08 03 A6 */	mtlr r0
/* 8000DED8 00009DF8  83 E1 00 C4 */	lwz r31, 0xc4(r1)
/* 8000DEDC 00009DFC  83 C1 00 C0 */	lwz r30, 0xc0(r1)
/* 8000DEE0 00009E00  38 21 00 D8 */	addi r1, r1, 0xd8
/* 8000DEE4 00009E04  4E 80 00 20 */	blr

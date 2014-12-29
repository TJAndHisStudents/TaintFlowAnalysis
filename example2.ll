; ModuleID = 'example.bc'

@R_EBX_32 = global i32 0
@R_ESP_32 = global i32 0
@R_CF = global i1 false
@R_OF = global i1 false
@R_AF = global i1 false
@R_PF = global i1 false
@R_SF = global i1 false
@R_ZF = global i1 false
@R_EAX_32 = global i32 0
@R_EBP_32 = global i32 0
@R_ESI_32 = global i32 0
@R_ECX_32 = global i32 0
@R_EDX_32 = global i32 0
@R_EDI_32 = global i32 0
@R_GS_BASE_32 = global i32 0

declare void @fake_assert(i32)

declare void @set_memory(i64, i8)

declare void @set_memory_multi(i64, i8*, i32)

declare i8 @get_memory(i64)

declare void @get_memory_multi(i64, i8*, i32)

define i32 @0() {
allocs:
  br label %BB_0

invalid_address:                                  ; preds = %switch
  call void @fake_assert(i32 0)
  unreachable

switch:                                           ; preds = %BB_10, %BB_12, %BB_15, %BB_18, %BB_21, %BB_24, %BB_27, %BB_45, %BB_50, %BB_58, %BB_60, %BB_70, %BB_79, %BB_81, %BB_92, %BB_103, %BB_122, %BB_127, %BB_145, %BB_198, %BB_221, %BB_230, %BB_232, %BB_239
  %read_addr.in = phi i32 [ %load2224, %BB_239 ], [ %load2097, %BB_232 ], [ %load2091, %BB_230 ], [ %load1951, %BB_221 ], [ %load1675, %BB_198 ], [ %load1436, %BB_145 ], [ %load1298, %BB_127 ], [ %load1277, %BB_122 ], [ %load_var_R_EAX_321106, %BB_103 ], [ %load1024, %BB_92 ], [ %load933, %BB_81 ], [ %load_var_R_EDX_32919, %BB_79 ], [ %load852, %BB_70 ], [ %load521, %BB_60 ], [ %load_var_R_EAX_32507, %BB_58 ], [ %load445, %BB_50 ], [ %load372, %BB_45 ], [ %load300, %BB_27 ], [ %load291, %BB_24 ], [ %load282, %BB_21 ], [ %load273, %BB_18 ], [ %load264, %BB_15 ], [ %load141, %BB_12 ], [ %load133, %BB_10 ]
  %read_addr = zext i32 %read_addr.in to i64
  switch i64 %read_addr, label %invalid_address [
    i64 134513492, label %BB_0
    i64 134513493, label %BB_1
    i64 134513496, label %BB_2
    i64 134513501, label %BB_3
    i64 134513507, label %BB_4
    i64 134513513, label %switch.BB_5_crit_edge
    i64 134513515, label %BB_6
    i64 134513517, label %BB_7
    i64 134513522, label %BB_8
    i64 134513525, label %BB_9
    i64 134513526, label %switch.BB_10_crit_edge
    i64 134513536, label %BB_11
    i64 134513542, label %BB_12
    i64 134513548, label %BB_13
    i64 134513550, label %BB_14
    i64 134513552, label %BB_15
    i64 134513558, label %BB_16
    i64 134513563, label %BB_11
    i64 134513568, label %BB_18
    i64 134513574, label %BB_19
    i64 134513579, label %BB_11
    i64 134513584, label %BB_21
    i64 134513590, label %BB_22
    i64 134513595, label %BB_11
    i64 134513600, label %BB_24
    i64 134513606, label %BB_25
    i64 134513611, label %BB_11
    i64 134513616, label %BB_27
    i64 134513622, label %BB_28
    i64 134513627, label %BB_11
    i64 134513632, label %BB_30
    i64 134513634, label %BB_31
    i64 134513635, label %switch.BB_32_crit_edge
    i64 134513637, label %BB_33
    i64 134513640, label %BB_34
    i64 134513641, label %BB_35
    i64 134513642, label %BB_36
    i64 134513643, label %BB_37
    i64 134513648, label %BB_38
    i64 134513653, label %BB_39
    i64 134513654, label %BB_40
    i64 134513655, label %BB_41
    i64 134513660, label %BB_42
    i64 134513666, label %BB_44
    i64 134513668, label %BB_44
    i64 134513670, label %BB_44
    i64 134513672, label %BB_44
    i64 134513674, label %BB_44
    i64 134513676, label %BB_44
    i64 134513678, label %BB_44
    i64 134513680, label %BB_44
    i64 134513683, label %BB_45
    i64 134513684, label %BB_46
    i64 134513686, label %BB_46
    i64 134513688, label %BB_46
    i64 134513690, label %BB_46
    i64 134513692, label %BB_46
    i64 134513694, label %BB_46
    i64 134513696, label %BB_46
    i64 134513701, label %switch.BB_47_crit_edge
    i64 134513706, label %BB_48
    i64 134513709, label %BB_49
    i64 134513711, label %BB_50
    i64 134513712, label %BB_51
    i64 134513717, label %switch.BB_52_crit_edge
    i64 134513719, label %BB_53
    i64 134513721, label %BB_54
    i64 134513722, label %BB_55
    i64 134513724, label %BB_56
    i64 134513727, label %BB_57
    i64 134513734, label %BB_58
    i64 134513736, label %BB_59
    i64 134513737, label %switch.BB_60_crit_edge
    i64 134513738, label %BB_62
    i64 134513744, label %BB_62
    i64 134513749, label %switch.BB_63_crit_edge
    i64 134513754, label %BB_64
    i64 134513757, label %BB_65
    i64 134513759, label %switch.BB_66_crit_edge
    i64 134513762, label %BB_67
    i64 134513764, label %BB_68
    i64 134513766, label %BB_69
    i64 134513768, label %BB_70
    i64 134513769, label %BB_71
    i64 134513774, label %switch.BB_72_crit_edge
    i64 134513776, label %BB_73
    i64 134513778, label %BB_74
    i64 134513779, label %BB_75
    i64 134513781, label %BB_76
    i64 134513784, label %BB_77
    i64 134513788, label %BB_78
    i64 134513795, label %BB_79
    i64 134513797, label %BB_80
    i64 134513798, label %switch.BB_81_crit_edge
    i64 134513799, label %BB_84
    i64 134513801, label %BB_84
    i64 134513808, label %BB_84
    i64 134513815, label %BB_85
    i64 134513817, label %BB_86
    i64 134513818, label %BB_87
    i64 134513820, label %BB_88
    i64 134513823, label %BB_89
    i64 134513828, label %BB_90
    i64 134513835, label %BB_91
    i64 134513836, label %BB_92
    i64 134513838, label %BB_93
    i64 134513840, label %BB_93
    i64 134513845, label %switch.BB_94_crit_edge
    i64 134513847, label %BB_95
    i64 134513849, label %BB_96
    i64 134513854, label %switch.BB_97_crit_edge
    i64 134513856, label %BB_98
    i64 134513858, label %BB_99
    i64 134513859, label %BB_100
    i64 134513861, label %BB_101
    i64 134513864, label %BB_102
    i64 134513871, label %BB_103
    i64 134513873, label %BB_104
    i64 134513874, label %BB_62
    i64 134513879, label %BB_62
    i64 134513880, label %BB_62
    i64 134513885, label %BB_107
    i64 134513886, label %BB_108
    i64 134513888, label %BB_109
    i64 134513891, label %BB_110
    i64 134513898, label %BB_111
    i64 134513905, label %BB_112
    i64 134513909, label %BB_113
    i64 134513911, label %BB_114
    i64 134513914, label %switch.BB_115_crit_edge
    i64 134513917, label %BB_118
    i64 134513919, label %BB_117
    i64 134513926, label %BB_118
    i64 134513929, label %BB_119
    i64 134513932, label %switch.BB_120_crit_edge
    i64 134513934, label %BB_121
    i64 134513935, label %switch.BB_122_crit_edge
    i64 134513936, label %BB_123
    i64 134513937, label %BB_124
    i64 134513939, label %switch.BB_125_crit_edge
    i64 134513942, label %BB_126
    i64 134513943, label %switch.BB_127_crit_edge
    i64 134513944, label %BB_128
    i64 134513945, label %BB_129
    i64 134513947, label %BB_130
    i64 134513950, label %BB_131
    i64 134513957, label %BB_132
    i64 134513964, label %BB_133
    i64 134513968, label %BB_134
    i64 134513970, label %BB_135
    i64 134513973, label %switch.BB_136_crit_edge
    i64 134513976, label %BB_137
    i64 134513981, label %BB_138
    i64 134513984, label %BB_143
    i64 134513986, label %BB_140
    i64 134513993, label %BB_141
    i64 134513998, label %BB_142
    i64 134514001, label %BB_143
    i64 134514004, label %BB_144
    i64 134514005, label %switch.BB_145_crit_edge
    i64 134514006, label %BB_146
    i64 134514007, label %BB_147
    i64 134514009, label %BB_148
    i64 134514012, label %BB_149
    i64 134514015, label %BB_150
    i64 134514018, label %switch.BB_151_crit_edge
    i64 134514022, label %BB_152
    i64 134514028, label %switch.BB_153_crit_edge
    i64 134514032, label %BB_154
    i64 134514034, label %BB_155
    i64 134514038, label %switch.BB_156_crit_edge
    i64 134514042, label %BB_157
    i64 134514050, label %BB_158
    i64 134514058, label %BB_159
    i64 134514066, label %BB_160
    i64 134514071, label %BB_161
    i64 134514076, label %BB_162
    i64 134514081, label %BB_163
    i64 134514085, label %switch.BB_164_crit_edge
    i64 134514089, label %BB_165
    i64 134514096, label %BB_166
    i64 134514101, label %BB_167
    i64 134514105, label %switch.BB_168_crit_edge
    i64 134514109, label %BB_169
    i64 134514116, label %BB_170
    i64 134514121, label %BB_171
    i64 134514125, label %switch.BB_172_crit_edge
    i64 134514128, label %BB_173
    i64 134514133, label %BB_174
    i64 134514137, label %BB_175
    i64 134514141, label %switch.BB_176_crit_edge
    i64 134514144, label %BB_177
    i64 134514149, label %BB_178
    i64 134514153, label %BB_179
    i64 134514157, label %switch.BB_180_crit_edge
    i64 134514160, label %switch.BB_181_crit_edge
    i64 134514164, label %BB_182
    i64 134514168, label %switch.BB_183_crit_edge
    i64 134514172, label %BB_184
    i64 134514180, label %BB_185
    i64 134514184, label %switch.BB_186_crit_edge
    i64 134514187, label %BB_187
    i64 134514192, label %BB_188
    i64 134514200, label %BB_189
    i64 134514204, label %switch.BB_190_crit_edge
    i64 134514207, label %BB_191
    i64 134514212, label %BB_192
    i64 134514217, label %BB_193
    i64 134514221, label %switch.BB_194_crit_edge
    i64 134514228, label %BB_195
    i64 134514230, label %BB_196
    i64 134514235, label %BB_197
    i64 134514236, label %switch.BB_198_crit_edge
    i64 134514237, label %BB_199
    i64 134514239, label %BB_199
    i64 134514240, label %BB_199
    i64 134514241, label %BB_200
    i64 134514242, label %BB_201
    i64 134514244, label %BB_202
    i64 134514245, label %BB_203
    i64 134514246, label %BB_204
    i64 134514251, label %BB_205
    i64 134514257, label %BB_206
    i64 134514260, label %BB_207
    i64 134514264, label %BB_208
    i64 134514270, label %BB_209
    i64 134514275, label %BB_210
    i64 134514281, label %switch.BB_211_crit_edge
    i64 134514283, label %BB_212
    i64 134514286, label %BB_213
    i64 134514288, label %BB_214
    i64 134514290, label %BB_216
    i64 134514296, label %BB_216
    i64 134514300, label %BB_217
    i64 134514303, label %BB_218
    i64 134514307, label %BB_219
    i64 134514311, label %switch.BB_220_crit_edge
    i64 134514315, label %BB_221
    i64 134514322, label %BB_222
    i64 134514325, label %BB_223
    i64 134514327, label %BB_224
    i64 134514329, label %BB_225
    i64 134514332, label %BB_226
    i64 134514333, label %switch.BB_227_crit_edge
    i64 134514334, label %switch.BB_228_crit_edge
    i64 134514335, label %switch.BB_229_crit_edge
    i64 134514336, label %switch.BB_230_crit_edge
    i64 134514337, label %BB_232
    i64 134514339, label %BB_232
    i64 134514340, label %BB_232
    i64 134514341, label %BB_232
    i64 134514342, label %BB_232
    i64 134514343, label %BB_232
    i64 134514344, label %BB_232
    i64 134514345, label %BB_232
    i64 134514346, label %BB_232
    i64 134514347, label %BB_232
    i64 134514348, label %BB_232
    i64 134514349, label %BB_232
    i64 134514350, label %BB_232
    i64 134514351, label %BB_232
    i64 134514352, label %BB_232
    i64 134514356, label %BB_233
    i64 134514357, label %BB_234
    i64 134514360, label %BB_235
    i64 134514365, label %BB_236
    i64 134514371, label %BB_237
    i64 134514374, label %BB_238
    i64 134514375, label %switch.BB_239_crit_edge
    i64 134513665, label %BB_43
  ]

switch.BB_5_crit_edge:                            ; preds = %switch
  %load_var_R_EAX_32.pre = load i32* @R_EAX_32, align 4
  br label %BB_5

switch.BB_10_crit_edge:                           ; preds = %switch
  %load_var_R_ESP_32131.pre = load i32* @R_ESP_32, align 4
  br label %BB_10

switch.BB_32_crit_edge:                           ; preds = %switch
  %load_var_R_ESP_32311.pre = load i32* @R_ESP_32, align 4
  br label %BB_32

switch.BB_47_crit_edge:                           ; preds = %switch
  %load_var_R_EAX_32376.pre = load i32* @R_EAX_32, align 4
  br label %BB_47

switch.BB_52_crit_edge:                           ; preds = %switch
  %load_var_R_EAX_32449.pre = load i32* @R_EAX_32, align 4
  br label %BB_52

switch.BB_60_crit_edge:                           ; preds = %switch
  %load_var_R_ESP_32519.pre = load i32* @R_ESP_32, align 4
  br label %BB_60

switch.BB_63_crit_edge:                           ; preds = %switch
  %load_var_R_EAX_32527.pre = load i32* @R_EAX_32, align 4
  br label %BB_63

switch.BB_66_crit_edge:                           ; preds = %switch
  %load_var_R_EDX_32639.pre = load i32* @R_EDX_32, align 4
  br label %BB_66

switch.BB_72_crit_edge:                           ; preds = %switch
  %load_var_R_EDX_32856.pre = load i32* @R_EDX_32, align 4
  br label %BB_72

switch.BB_81_crit_edge:                           ; preds = %switch
  %load_var_R_ESP_32931.pre = load i32* @R_ESP_32, align 4
  br label %BB_81

switch.BB_94_crit_edge:                           ; preds = %switch
  %load_var_R_EAX_321029.pre = load i32* @R_EAX_32, align 4
  br label %BB_94

switch.BB_97_crit_edge:                           ; preds = %switch
  %load_var_R_EAX_321047.pre = load i32* @R_EAX_32, align 4
  br label %BB_97

switch.BB_120_crit_edge:                          ; preds = %switch
  %load_var_R_EDX_321232.pre = load i32* @R_EDX_32, align 4
  br label %BB_120

switch.BB_122_crit_edge:                          ; preds = %switch
  %load_var_R_ESP_321275.pre = load i32* @R_ESP_32, align 4
  br label %BB_122

switch.BB_115_crit_edge:                          ; preds = %switch
  %load_var_R_EAX_321219.pre = load i32* @R_EAX_32, align 4
  br label %BB_115

switch.BB_125_crit_edge:                          ; preds = %switch
  %load_var_R_EBP_321287.pre = load i32* @R_EBP_32, align 4
  br label %BB_125

switch.BB_127_crit_edge:                          ; preds = %switch
  %load_var_R_ESP_321296.pre = load i32* @R_ESP_32, align 4
  br label %BB_127

switch.BB_136_crit_edge:                          ; preds = %switch
  %load_var_R_EAX_321405.pre = load i32* @R_EAX_32, align 4
  br label %BB_136

switch.BB_145_crit_edge:                          ; preds = %switch
  %load_var_R_ESP_321434.pre = load i32* @R_ESP_32, align 4
  br label %BB_145

switch.BB_151_crit_edge:                          ; preds = %switch
  %load_var_R_EAX_321503.pre = load i32* @R_EAX_32, align 4
  br label %BB_151

switch.BB_153_crit_edge:                          ; preds = %switch
  %load_var_R_EAX_321510.pre = load i32* @R_EAX_32, align 4
  br label %BB_153

switch.BB_156_crit_edge:                          ; preds = %switch
  %load_var_R_EAX_321516.pre = load i32* @R_EAX_32, align 4
  br label %BB_156

switch.BB_164_crit_edge:                          ; preds = %switch
  %load_var_R_EAX_321542.pre = load i32* @R_EAX_32, align 4
  br label %BB_164

switch.BB_168_crit_edge:                          ; preds = %switch
  %load_var_R_EAX_321554.pre = load i32* @R_EAX_32, align 4
  br label %BB_168

switch.BB_172_crit_edge:                          ; preds = %switch
  %load_var_R_EAX_321567.pre = load i32* @R_EAX_32, align 4
  br label %BB_172

switch.BB_176_crit_edge:                          ; preds = %switch
  %load_var_R_EAX_321582.pre = load i32* @R_EAX_32, align 4
  br label %BB_176

switch.BB_180_crit_edge:                          ; preds = %switch
  %load_var_R_EAX_321595.pre = load i32* @R_EAX_32, align 4
  br label %BB_180

switch.BB_181_crit_edge:                          ; preds = %switch
  %load_var_R_EAX_321602.pre = load i32* @R_EAX_32, align 4
  br label %BB_181

switch.BB_183_crit_edge:                          ; preds = %switch
  %load_var_R_EAX_321610.pre = load i32* @R_EAX_32, align 4
  br label %BB_183

switch.BB_186_crit_edge:                          ; preds = %switch
  %load_var_R_EAX_321620.pre = load i32* @R_EAX_32, align 4
  br label %BB_186

switch.BB_190_crit_edge:                          ; preds = %switch
  %load_var_R_EAX_321632.pre = load i32* @R_EAX_32, align 4
  br label %BB_190

switch.BB_194_crit_edge:                          ; preds = %switch
  %load_var_R_EDX_321641.pre = load i32* @R_EDX_32, align 4
  br label %BB_194

switch.BB_198_crit_edge:                          ; preds = %switch
  %load_var_R_ESP_321673.pre = load i32* @R_ESP_32, align 4
  br label %BB_198

switch.BB_211_crit_edge:                          ; preds = %switch
  %load_var_R_EAX_321787.pre = load i32* @R_EAX_32, align 4
  br label %BB_211

switch.BB_227_crit_edge:                          ; preds = %switch
  %load_var_R_ESP_322074.pre = load i32* @R_ESP_32, align 4
  br label %BB_227

switch.BB_228_crit_edge:                          ; preds = %switch
  %load_var_R_ESP_322079.pre = load i32* @R_ESP_32, align 4
  br label %BB_228

switch.BB_229_crit_edge:                          ; preds = %switch
  %load_var_R_ESP_322084.pre = load i32* @R_ESP_32, align 4
  br label %BB_229

switch.BB_230_crit_edge:                          ; preds = %switch
  %load_var_R_ESP_322089.pre = load i32* @R_ESP_32, align 4
  br label %BB_230

switch.BB_220_crit_edge:                          ; preds = %switch
  %load_var_R_EAX_321944.pre = load i32* @R_EAX_32, align 4
  br label %BB_220

switch.BB_239_crit_edge:                          ; preds = %switch
  %load_var_R_ESP_322222.pre = load i32* @R_ESP_32, align 4
  br label %BB_239

BB_0:                                             ; preds = %BB_209, %switch, %allocs
  %load_var_R_EBX_32 = load i32* @R_EBX_32, align 4
  %load_var_R_ESP_32 = load i32* @R_ESP_32, align 4
  %-_tmp = add i32 %load_var_R_ESP_32, -4
  store i32 %-_tmp, i32* @R_ESP_32, align 4
  %load_address = inttoptr i32 %-_tmp to i32*
  store i32 %load_var_R_EBX_32, i32* %load_address, align 4
  br label %BB_1

BB_239:                                           ; preds = %BB_238, %switch.BB_239_crit_edge
  %load_var_R_ESP_322222 = phi i32 [ %load_var_R_ESP_322222.pre, %switch.BB_239_crit_edge ], [ %"+_tmp2221", %BB_238 ]
  %load_address2223 = inttoptr i32 %load_var_R_ESP_322222 to i32*
  %load2224 = load i32* %load_address2223, align 4
  %"+_tmp2226" = add i32 %load_var_R_ESP_322222, 4
  store i32 %"+_tmp2226", i32* @R_ESP_32, align 4
  br label %switch

BB_238:                                           ; preds = %BB_237, %switch
  %load_var_R_ESP_322217 = load i32* @R_ESP_32, align 4
  %load_address2218 = inttoptr i32 %load_var_R_ESP_322217 to i32*
  %load2219 = load i32* %load_address2218, align 4
  store i32 %load2219, i32* @R_EBX_32, align 4
  %"+_tmp2221" = add i32 %load_var_R_ESP_322217, 4
  store i32 %"+_tmp2221", i32* @R_ESP_32, align 4
  br label %BB_239

BB_237:                                           ; preds = %BB_236, %switch
  %load_var_R_ESP_322180 = load i32* @R_ESP_32, align 4
  %uadd = call { i32, i1 } @llvm.uadd.with.overflow.i32(i32 %load_var_R_ESP_322180, i32 8)
  %0 = extractvalue { i32, i1 } %uadd, 0
  store i32 %0, i32* @R_ESP_32, align 4
  %"<_tmp2184" = extractvalue { i32, i1 } %uadd, 1
  store i1 %"<_tmp2184", i1* @R_CF, align 1
  %"^_tmp2188" = xor i32 %load_var_R_ESP_322180, -2147483648
  %"^_tmp2191" = xor i32 %0, %load_var_R_ESP_322180
  %"&_tmp2192" = and i32 %"^_tmp2191", %"^_tmp2188"
  %cast_high2194 = icmp slt i32 %"&_tmp2192", 0
  store i1 %cast_high2194, i1* @R_OF, align 1
  %load_var_R_ESP_322195 = load i32* @R_ESP_32, align 4
  %"^_tmp2197" = xor i32 %load_var_R_ESP_322195, %load_var_R_ESP_322180
  %"&_tmp2200" = and i32 %"^_tmp2197", 16
  %"==_tmp2201" = icmp ne i32 %"&_tmp2200", 0
  store i1 %"==_tmp2201", i1* @R_AF, align 1
  %">>_tmp2203" = lshr i32 %load_var_R_ESP_322195, 4
  %"^_tmp2205" = xor i32 %">>_tmp2203", %load_var_R_ESP_322195
  %">>_tmp2206" = lshr i32 %"^_tmp2205", 2
  %"^_tmp2207" = xor i32 %">>_tmp2206", %"^_tmp2205"
  %">>_tmp2208" = lshr i32 %"^_tmp2207", 1
  %"^_tmp2209" = xor i32 %">>_tmp2208", %"^_tmp2207"
  %1 = and i32 %"^_tmp2209", 1
  %bwnot2211 = icmp eq i32 %1, 0
  store i1 %bwnot2211, i1* @R_PF, align 1
  %load_var_R_ESP_322212 = load i32* @R_ESP_32, align 4
  %cast_high2214 = icmp slt i32 %load_var_R_ESP_322212, 0
  store i1 %cast_high2214, i1* @R_SF, align 1
  %"==_tmp2216" = icmp eq i32 %load_var_R_ESP_322212, 0
  store i1 %"==_tmp2216", i1* @R_ZF, align 1
  br label %BB_238

BB_236:                                           ; preds = %switch
  %load_var_R_EBX_322143 = load i32* @R_EBX_32, align 4
  %uadd2229 = call { i32, i1 } @llvm.uadd.with.overflow.i32(i32 %load_var_R_EBX_322143, i32 6467)
  %2 = extractvalue { i32, i1 } %uadd2229, 0
  store i32 %2, i32* @R_EBX_32, align 4
  %"<_tmp2147" = extractvalue { i32, i1 } %uadd2229, 1
  store i1 %"<_tmp2147", i1* @R_CF, align 1
  %"^_tmp2151" = xor i32 %load_var_R_EBX_322143, -2147483648
  %"^_tmp2154" = xor i32 %2, %load_var_R_EBX_322143
  %"&_tmp2155" = and i32 %"^_tmp2154", %"^_tmp2151"
  %cast_high2157 = icmp slt i32 %"&_tmp2155", 0
  store i1 %cast_high2157, i1* @R_OF, align 1
  %load_var_R_EBX_322158 = load i32* @R_EBX_32, align 4
  %"^_tmp2160" = xor i32 %load_var_R_EBX_322158, %load_var_R_EBX_322143
  %"&_tmp2163" = and i32 %"^_tmp2160", 16
  %"==_tmp2164" = icmp ne i32 %"&_tmp2163", 0
  store i1 %"==_tmp2164", i1* @R_AF, align 1
  %">>_tmp2166" = lshr i32 %load_var_R_EBX_322158, 4
  %"^_tmp2168" = xor i32 %">>_tmp2166", %load_var_R_EBX_322158
  %">>_tmp2169" = lshr i32 %"^_tmp2168", 2
  %"^_tmp2170" = xor i32 %">>_tmp2169", %"^_tmp2168"
  %">>_tmp2171" = lshr i32 %"^_tmp2170", 1
  %"^_tmp2172" = xor i32 %">>_tmp2171", %"^_tmp2170"
  %3 = and i32 %"^_tmp2172", 1
  %bwnot2174 = icmp eq i32 %3, 0
  store i1 %bwnot2174, i1* @R_PF, align 1
  %load_var_R_EBX_322175 = load i32* @R_EBX_32, align 4
  %cast_high2177 = icmp slt i32 %load_var_R_EBX_322175, 0
  store i1 %cast_high2177, i1* @R_SF, align 1
  %"==_tmp2179" = icmp eq i32 %load_var_R_EBX_322175, 0
  store i1 %"==_tmp2179", i1* @R_ZF, align 1
  br label %BB_237

BB_235:                                           ; preds = %BB_234, %switch
  %load_var_R_ESP_322139 = load i32* @R_ESP_32, align 4
  %-_tmp2140 = add i32 %load_var_R_ESP_322139, -4
  store i32 %-_tmp2140, i32* @R_ESP_32, align 4
  %load_address2142 = inttoptr i32 %-_tmp2140 to i32*
  store i32 134514365, i32* %load_address2142, align 4
  br label %BB_44

BB_234:                                           ; preds = %BB_233, %switch
  %load_var_R_ESP_322106 = load i32* @R_ESP_32, align 4
  %-_tmp2108 = add i32 %load_var_R_ESP_322106, -8
  store i32 %-_tmp2108, i32* @R_ESP_32, align 4
  %"<_tmp2109" = icmp ult i32 %load_var_R_ESP_322106, 8
  store i1 %"<_tmp2109", i1* @R_CF, align 1
  %4 = sub i32 7, %load_var_R_ESP_322106
  %"&_tmp2115" = and i32 %4, %load_var_R_ESP_322106
  %cast_high2117 = icmp slt i32 %"&_tmp2115", 0
  store i1 %cast_high2117, i1* @R_OF, align 1
  %load_var_R_ESP_322118 = load i32* @R_ESP_32, align 4
  %"^_tmp2120" = xor i32 %load_var_R_ESP_322118, %load_var_R_ESP_322106
  %"&_tmp2122" = and i32 %"^_tmp2120", 16
  %"==_tmp2123" = icmp ne i32 %"&_tmp2122", 0
  store i1 %"==_tmp2123", i1* @R_AF, align 1
  %">>_tmp2125" = lshr i32 %load_var_R_ESP_322118, 4
  %"^_tmp2127" = xor i32 %">>_tmp2125", %load_var_R_ESP_322118
  %">>_tmp2128" = lshr i32 %"^_tmp2127", 2
  %"^_tmp2129" = xor i32 %">>_tmp2128", %"^_tmp2127"
  %">>_tmp2130" = lshr i32 %"^_tmp2129", 1
  %"^_tmp2131" = xor i32 %">>_tmp2130", %"^_tmp2129"
  %5 = and i32 %"^_tmp2131", 1
  %bwnot2133 = icmp eq i32 %5, 0
  store i1 %bwnot2133, i1* @R_PF, align 1
  %load_var_R_ESP_322134 = load i32* @R_ESP_32, align 4
  %cast_high2136 = icmp slt i32 %load_var_R_ESP_322134, 0
  store i1 %cast_high2136, i1* @R_SF, align 1
  %"==_tmp2138" = icmp eq i32 %load_var_R_ESP_322134, 0
  store i1 %"==_tmp2138", i1* @R_ZF, align 1
  br label %BB_235

BB_233:                                           ; preds = %switch
  %load_var_R_EBX_322101 = load i32* @R_EBX_32, align 4
  %load_var_R_ESP_322102 = load i32* @R_ESP_32, align 4
  %-_tmp2103 = add i32 %load_var_R_ESP_322102, -4
  store i32 %-_tmp2103, i32* @R_ESP_32, align 4
  %load_address2105 = inttoptr i32 %-_tmp2103 to i32*
  store i32 %load_var_R_EBX_322101, i32* %load_address2105, align 4
  br label %BB_234

BB_232:                                           ; preds = %switch, %switch, %switch, %switch, %switch, %switch, %switch, %switch, %switch, %switch, %switch, %switch, %switch, %switch, %switch
  %load_var_R_ESP_322095 = load i32* @R_ESP_32, align 4
  %load_address2096 = inttoptr i32 %load_var_R_ESP_322095 to i32*
  %load2097 = load i32* %load_address2096, align 4
  %"+_tmp2099" = add i32 %load_var_R_ESP_322095, 4
  store i32 %"+_tmp2099", i32* @R_ESP_32, align 4
  br label %switch

BB_230:                                           ; preds = %BB_229, %switch.BB_230_crit_edge
  %load_var_R_ESP_322089 = phi i32 [ %load_var_R_ESP_322089.pre, %switch.BB_230_crit_edge ], [ %"+_tmp2088", %BB_229 ]
  %load_address2090 = inttoptr i32 %load_var_R_ESP_322089 to i32*
  %load2091 = load i32* %load_address2090, align 4
  %"+_tmp2093" = add i32 %load_var_R_ESP_322089, 4
  store i32 %"+_tmp2093", i32* @R_ESP_32, align 4
  br label %switch

BB_229:                                           ; preds = %BB_228, %switch.BB_229_crit_edge
  %load_var_R_ESP_322084 = phi i32 [ %load_var_R_ESP_322084.pre, %switch.BB_229_crit_edge ], [ %"+_tmp2083", %BB_228 ]
  %load_address2085 = inttoptr i32 %load_var_R_ESP_322084 to i32*
  %load2086 = load i32* %load_address2085, align 4
  store i32 %load2086, i32* @R_EBP_32, align 4
  %"+_tmp2088" = add i32 %load_var_R_ESP_322084, 4
  store i32 %"+_tmp2088", i32* @R_ESP_32, align 4
  br label %BB_230

BB_228:                                           ; preds = %BB_227, %switch.BB_228_crit_edge
  %load_var_R_ESP_322079 = phi i32 [ %load_var_R_ESP_322079.pre, %switch.BB_228_crit_edge ], [ %"+_tmp2078", %BB_227 ]
  %load_address2080 = inttoptr i32 %load_var_R_ESP_322079 to i32*
  %load2081 = load i32* %load_address2080, align 4
  store i32 %load2081, i32* @R_EDI_32, align 4
  %"+_tmp2083" = add i32 %load_var_R_ESP_322079, 4
  store i32 %"+_tmp2083", i32* @R_ESP_32, align 4
  br label %BB_229

BB_227:                                           ; preds = %BB_226, %switch.BB_227_crit_edge
  %load_var_R_ESP_322074 = phi i32 [ %load_var_R_ESP_322074.pre, %switch.BB_227_crit_edge ], [ %"+_tmp2073", %BB_226 ]
  %load_address2075 = inttoptr i32 %load_var_R_ESP_322074 to i32*
  %load2076 = load i32* %load_address2075, align 4
  store i32 %load2076, i32* @R_ESI_32, align 4
  %"+_tmp2078" = add i32 %load_var_R_ESP_322074, 4
  store i32 %"+_tmp2078", i32* @R_ESP_32, align 4
  br label %BB_228

BB_226:                                           ; preds = %BB_225, %switch
  %load_var_R_ESP_322069 = load i32* @R_ESP_32, align 4
  %load_address2070 = inttoptr i32 %load_var_R_ESP_322069 to i32*
  %load2071 = load i32* %load_address2070, align 4
  store i32 %load2071, i32* @R_EBX_32, align 4
  %"+_tmp2073" = add i32 %load_var_R_ESP_322069, 4
  store i32 %"+_tmp2073", i32* @R_ESP_32, align 4
  br label %BB_227

BB_225:                                           ; preds = %BB_214, %BB_224, %switch
  %load_var_R_ESP_322032 = load i32* @R_ESP_32, align 4
  %uadd2230 = call { i32, i1 } @llvm.uadd.with.overflow.i32(i32 %load_var_R_ESP_322032, i32 28)
  %6 = extractvalue { i32, i1 } %uadd2230, 0
  store i32 %6, i32* @R_ESP_32, align 4
  %"<_tmp2036" = extractvalue { i32, i1 } %uadd2230, 1
  store i1 %"<_tmp2036", i1* @R_CF, align 1
  %"^_tmp2040" = xor i32 %load_var_R_ESP_322032, -2147483648
  %"^_tmp2043" = xor i32 %6, %load_var_R_ESP_322032
  %"&_tmp2044" = and i32 %"^_tmp2043", %"^_tmp2040"
  %cast_high2046 = icmp slt i32 %"&_tmp2044", 0
  store i1 %cast_high2046, i1* @R_OF, align 1
  %load_var_R_ESP_322047 = load i32* @R_ESP_32, align 4
  %"^_tmp2049" = xor i32 %load_var_R_ESP_322047, %load_var_R_ESP_322032
  %"^_tmp2051" = and i32 %"^_tmp2049", 16
  %"==_tmp2053" = icmp eq i32 %"^_tmp2051", 0
  store i1 %"==_tmp2053", i1* @R_AF, align 1
  %">>_tmp2055" = lshr i32 %load_var_R_ESP_322047, 4
  %"^_tmp2057" = xor i32 %">>_tmp2055", %load_var_R_ESP_322047
  %">>_tmp2058" = lshr i32 %"^_tmp2057", 2
  %"^_tmp2059" = xor i32 %">>_tmp2058", %"^_tmp2057"
  %">>_tmp2060" = lshr i32 %"^_tmp2059", 1
  %"^_tmp2061" = xor i32 %">>_tmp2060", %"^_tmp2059"
  %7 = and i32 %"^_tmp2061", 1
  %bwnot2063 = icmp eq i32 %7, 0
  store i1 %bwnot2063, i1* @R_PF, align 1
  %load_var_R_ESP_322064 = load i32* @R_ESP_32, align 4
  %cast_high2066 = icmp slt i32 %load_var_R_ESP_322064, 0
  store i1 %cast_high2066, i1* @R_SF, align 1
  %"==_tmp2068" = icmp eq i32 %load_var_R_ESP_322064, 0
  store i1 %"==_tmp2068", i1* @R_ZF, align 1
  br label %BB_226

BB_224:                                           ; preds = %BB_223, %switch
  %load_var_R_ZF2030 = load i1* @R_ZF, align 1
  br i1 %load_var_R_ZF2030, label %BB_225, label %BB_216

BB_223:                                           ; preds = %BB_222, %switch
  %load_var_R_EDI_321994 = load i32* @R_EDI_32, align 4
  %load_var_R_ESI_321995 = load i32* @R_ESI_32, align 4
  %-_tmp1996 = sub i32 %load_var_R_EDI_321994, %load_var_R_ESI_321995
  %"<_tmp1999" = icmp ult i32 %load_var_R_EDI_321994, %load_var_R_ESI_321995
  store i1 %"<_tmp1999", i1* @R_CF, align 1
  %"^_tmp2002" = xor i32 %load_var_R_ESI_321995, %load_var_R_EDI_321994
  %"^_tmp2004" = xor i32 %-_tmp1996, %load_var_R_EDI_321994
  %"&_tmp2005" = and i32 %"^_tmp2004", %"^_tmp2002"
  %cast_high2007 = icmp slt i32 %"&_tmp2005", 0
  store i1 %cast_high2007, i1* @R_OF, align 1
  %load_var_R_EDI_322009 = load i32* @R_EDI_32, align 4
  %load_var_R_ESI_322011 = load i32* @R_ESI_32, align 4
  %"^_tmp2010" = xor i32 %-_tmp1996, %load_var_R_EDI_322009
  %"^_tmp2012" = xor i32 %"^_tmp2010", %load_var_R_ESI_322011
  %"&_tmp2013" = and i32 %"^_tmp2012", 16
  %"==_tmp2014" = icmp ne i32 %"&_tmp2013", 0
  store i1 %"==_tmp2014", i1* @R_AF, align 1
  %">>_tmp2016" = lshr i32 %-_tmp1996, 4
  %"^_tmp2018" = xor i32 %">>_tmp2016", %-_tmp1996
  %">>_tmp2019" = lshr i32 %"^_tmp2018", 2
  %"^_tmp2020" = xor i32 %">>_tmp2019", %"^_tmp2018"
  %">>_tmp2021" = lshr i32 %"^_tmp2020", 1
  %"^_tmp2022" = xor i32 %">>_tmp2021", %"^_tmp2020"
  %8 = and i32 %"^_tmp2022", 1
  %bwnot2024 = icmp eq i32 %8, 0
  store i1 %bwnot2024, i1* @R_PF, align 1
  %cast_high2027 = icmp slt i32 %-_tmp1996, 0
  store i1 %cast_high2027, i1* @R_SF, align 1
  %"==_tmp2029" = icmp eq i32 %load_var_R_EDI_321994, %load_var_R_ESI_321995
  store i1 %"==_tmp2029", i1* @R_ZF, align 1
  br label %BB_224

BB_222:                                           ; preds = %switch
  %load_var_R_EDI_321957 = load i32* @R_EDI_32, align 4
  %uadd2231 = call { i32, i1 } @llvm.uadd.with.overflow.i32(i32 %load_var_R_EDI_321957, i32 1)
  %9 = extractvalue { i32, i1 } %uadd2231, 0
  store i32 %9, i32* @R_EDI_32, align 4
  %"<_tmp1961" = extractvalue { i32, i1 } %uadd2231, 1
  store i1 %"<_tmp1961", i1* @R_CF, align 1
  %"^_tmp1965" = xor i32 %load_var_R_EDI_321957, -2147483648
  %"^_tmp1968" = xor i32 %9, %load_var_R_EDI_321957
  %"&_tmp1969" = and i32 %"^_tmp1968", %"^_tmp1965"
  %cast_high1971 = icmp slt i32 %"&_tmp1969", 0
  store i1 %cast_high1971, i1* @R_OF, align 1
  %load_var_R_EDI_321972 = load i32* @R_EDI_32, align 4
  %"^_tmp1974" = xor i32 %load_var_R_EDI_321972, %load_var_R_EDI_321957
  %"&_tmp1977" = and i32 %"^_tmp1974", 16
  %"==_tmp1978" = icmp ne i32 %"&_tmp1977", 0
  store i1 %"==_tmp1978", i1* @R_AF, align 1
  %">>_tmp1980" = lshr i32 %load_var_R_EDI_321972, 4
  %"^_tmp1982" = xor i32 %">>_tmp1980", %load_var_R_EDI_321972
  %">>_tmp1983" = lshr i32 %"^_tmp1982", 2
  %"^_tmp1984" = xor i32 %">>_tmp1983", %"^_tmp1982"
  %">>_tmp1985" = lshr i32 %"^_tmp1984", 1
  %"^_tmp1986" = xor i32 %">>_tmp1985", %"^_tmp1984"
  %10 = and i32 %"^_tmp1986", 1
  %bwnot1988 = icmp eq i32 %10, 0
  store i1 %bwnot1988, i1* @R_PF, align 1
  %load_var_R_EDI_321989 = load i32* @R_EDI_32, align 4
  %cast_high1991 = icmp slt i32 %load_var_R_EDI_321989, 0
  store i1 %cast_high1991, i1* @R_SF, align 1
  %"==_tmp1993" = icmp eq i32 %load_var_R_EDI_321989, 0
  store i1 %"==_tmp1993", i1* @R_ZF, align 1
  br label %BB_223

BB_221:                                           ; preds = %BB_220, %switch
  %load_var_R_EBX_321945 = load i32* @R_EBX_32, align 4
  %load_var_R_EDI_321946 = load i32* @R_EDI_32, align 4
  %"<<_tmp1947" = mul i32 %load_var_R_EDI_321946, 4
  %"+_tmp1948" = add i32 %load_var_R_EBX_321945, -248
  %"+_tmp1949" = add i32 %"+_tmp1948", %"<<_tmp1947"
  %load_address1950 = inttoptr i32 %"+_tmp1949" to i32*
  %load1951 = load i32* %load_address1950, align 4
  %load_var_R_ESP_321952 = load i32* @R_ESP_32, align 4
  %-_tmp1953 = add i32 %load_var_R_ESP_321952, -4
  store i32 %-_tmp1953, i32* @R_ESP_32, align 4
  %load_address1955 = inttoptr i32 %-_tmp1953 to i32*
  store i32 134514322, i32* %load_address1955, align 4
  br label %switch

BB_220:                                           ; preds = %BB_219, %switch.BB_220_crit_edge
  %load_var_R_EAX_321944 = phi i32 [ %load_var_R_EAX_321944.pre, %switch.BB_220_crit_edge ], [ %load1940, %BB_219 ]
  %load_var_R_ESP_321941 = load i32* @R_ESP_32, align 4
  %"+_tmp1942" = add i32 %load_var_R_ESP_321941, 4
  %load_address1943 = inttoptr i32 %"+_tmp1942" to i32*
  store i32 %load_var_R_EAX_321944, i32* %load_address1943, align 4
  br label %BB_221

BB_219:                                           ; preds = %BB_218, %switch
  %load_var_R_ESP_321937 = load i32* @R_ESP_32, align 4
  %"+_tmp1938" = add i32 %load_var_R_ESP_321937, 52
  %load_address1939 = inttoptr i32 %"+_tmp1938" to i32*
  %load1940 = load i32* %load_address1939, align 4
  store i32 %load1940, i32* @R_EAX_32, align 4
  br label %BB_220

BB_218:                                           ; preds = %BB_217, %switch
  %load_var_R_ESP_321933 = load i32* @R_ESP_32, align 4
  %"+_tmp1934" = add i32 %load_var_R_ESP_321933, 8
  %load_address1935 = inttoptr i32 %"+_tmp1934" to i32*
  %load_var_R_EAX_321936 = load i32* @R_EAX_32, align 4
  store i32 %load_var_R_EAX_321936, i32* %load_address1935, align 4
  br label %BB_219

BB_217:                                           ; preds = %BB_216, %switch
  %load_var_R_ESP_321930 = load i32* @R_ESP_32, align 4
  %load_address1931 = inttoptr i32 %load_var_R_ESP_321930 to i32*
  %load_var_R_EBP_321932 = load i32* @R_EBP_32, align 4
  store i32 %load_var_R_EBP_321932, i32* %load_address1931, align 4
  br label %BB_218

BB_216:                                           ; preds = %BB_214, %BB_224, %switch, %switch
  %load_var_R_ESP_321926 = load i32* @R_ESP_32, align 4
  %"+_tmp1927" = add i32 %load_var_R_ESP_321926, 56
  %load_address1928 = inttoptr i32 %"+_tmp1927" to i32*
  %load1929 = load i32* %load_address1928, align 4
  store i32 %load1929, i32* @R_EAX_32, align 4
  br label %BB_217

BB_214:                                           ; preds = %BB_213, %switch
  %load_var_R_ZF1923 = load i1* @R_ZF, align 1
  br i1 %load_var_R_ZF1923, label %BB_225, label %BB_216

BB_213:                                           ; preds = %BB_212, %switch
  %load_var_R_ESI_321906 = load i32* @R_ESI_32, align 4
  store i1 false, i1* @R_OF, align 1
  store i1 false, i1* @R_CF, align 1
  %">>_tmp1909" = lshr i32 %load_var_R_ESI_321906, 4
  %"^_tmp1911" = xor i32 %">>_tmp1909", %load_var_R_ESI_321906
  %">>_tmp1912" = lshr i32 %"^_tmp1911", 2
  %"^_tmp1913" = xor i32 %">>_tmp1912", %"^_tmp1911"
  %">>_tmp1914" = lshr i32 %"^_tmp1913", 1
  %"^_tmp1915" = xor i32 %">>_tmp1914", %"^_tmp1913"
  %11 = and i32 %"^_tmp1915", 1
  %bwnot1917 = icmp eq i32 %11, 0
  store i1 %bwnot1917, i1* @R_PF, align 1
  %cast_high1920 = icmp slt i32 %load_var_R_ESI_321906, 0
  store i1 %cast_high1920, i1* @R_SF, align 1
  %"==_tmp1922" = icmp eq i32 %load_var_R_ESI_321906, 0
  store i1 %"==_tmp1922", i1* @R_ZF, align 1
  br label %BB_214

BB_212:                                           ; preds = %BB_211, %switch
  %load_var_R_ESI_321822 = load i32* @R_ESI_32, align 4
  %"$>>_tmp1824" = ashr i32 %load_var_R_ESI_321822, 2
  store i32 %"$>>_tmp1824", i32* @R_ESI_32, align 4
  %12 = and i32 %load_var_R_ESI_321822, 2
  %cast_high1835 = icmp ne i32 %12, 0
  store i1 %cast_high1835, i1* @R_CF, align 1
  store i1 false, i1* @R_OF, align 1
  %cast_high1864 = icmp slt i32 %"$>>_tmp1824", 0
  store i1 %cast_high1864, i1* @R_SF, align 1
  %"==_tmp1875" = icmp eq i32 %"$>>_tmp1824", 0
  store i1 %"==_tmp1875", i1* @R_ZF, align 1
  %">>_tmp1886" = lshr i32 %"$>>_tmp1824", 4
  %"^_tmp1888" = xor i32 %">>_tmp1886", %"$>>_tmp1824"
  %">>_tmp1889" = lshr i32 %"^_tmp1888", 2
  %"^_tmp1890" = xor i32 %">>_tmp1889", %"^_tmp1888"
  %">>_tmp1891" = lshr i32 %"^_tmp1890", 1
  %"^_tmp1892" = xor i32 %">>_tmp1891", %"^_tmp1890"
  %13 = and i32 %"^_tmp1892", 1
  %bwnot1894 = icmp eq i32 %13, 0
  store i1 %bwnot1894, i1* @R_PF, align 1
  store i1 false, i1* @R_AF, align 1
  br label %BB_213

BB_211:                                           ; preds = %BB_210, %switch.BB_211_crit_edge
  %load_var_R_EAX_321787 = phi i32 [ %load_var_R_EAX_321787.pre, %switch.BB_211_crit_edge ], [ %"+_tmp1784", %BB_210 ]
  %load_var_R_ESI_321785 = load i32* @R_ESI_32, align 4
  %-_tmp1788 = sub i32 %load_var_R_ESI_321785, %load_var_R_EAX_321787
  store i32 %-_tmp1788, i32* @R_ESI_32, align 4
  %"<_tmp1790" = icmp ult i32 %load_var_R_ESI_321785, %load_var_R_EAX_321787
  store i1 %"<_tmp1790", i1* @R_CF, align 1
  %"^_tmp1793" = xor i32 %load_var_R_ESI_321785, %load_var_R_EAX_321787
  %"^_tmp1796" = xor i32 %-_tmp1788, %load_var_R_ESI_321785
  %"&_tmp1797" = and i32 %"^_tmp1796", %"^_tmp1793"
  %cast_high1799 = icmp slt i32 %"&_tmp1797", 0
  store i1 %cast_high1799, i1* @R_OF, align 1
  %load_var_R_ESI_321800 = load i32* @R_ESI_32, align 4
  %load_var_R_EAX_321803 = load i32* @R_EAX_32, align 4
  %"^_tmp1802" = xor i32 %load_var_R_ESI_321800, %load_var_R_ESI_321785
  %"^_tmp1804" = xor i32 %"^_tmp1802", %load_var_R_EAX_321803
  %"&_tmp1805" = and i32 %"^_tmp1804", 16
  %"==_tmp1806" = icmp ne i32 %"&_tmp1805", 0
  store i1 %"==_tmp1806", i1* @R_AF, align 1
  %load_var_R_ESI_321807 = load i32* @R_ESI_32, align 4
  %">>_tmp1808" = lshr i32 %load_var_R_ESI_321807, 4
  %"^_tmp1810" = xor i32 %">>_tmp1808", %load_var_R_ESI_321807
  %">>_tmp1811" = lshr i32 %"^_tmp1810", 2
  %"^_tmp1812" = xor i32 %">>_tmp1811", %"^_tmp1810"
  %">>_tmp1813" = lshr i32 %"^_tmp1812", 1
  %"^_tmp1814" = xor i32 %">>_tmp1813", %"^_tmp1812"
  %14 = and i32 %"^_tmp1814", 1
  %bwnot1816 = icmp eq i32 %14, 0
  store i1 %bwnot1816, i1* @R_PF, align 1
  %load_var_R_ESI_321817 = load i32* @R_ESI_32, align 4
  %cast_high1819 = icmp slt i32 %load_var_R_ESI_321817, 0
  store i1 %cast_high1819, i1* @R_SF, align 1
  %"==_tmp1821" = icmp eq i32 %load_var_R_ESI_321817, 0
  store i1 %"==_tmp1821", i1* @R_ZF, align 1
  br label %BB_212

BB_210:                                           ; preds = %switch
  %load_var_R_EBX_321783 = load i32* @R_EBX_32, align 4
  %"+_tmp1784" = add i32 %load_var_R_EBX_321783, -248
  store i32 %"+_tmp1784", i32* @R_EAX_32, align 4
  br label %BB_211

BB_209:                                           ; preds = %BB_208, %switch
  %load_var_R_ESP_321779 = load i32* @R_ESP_32, align 4
  %-_tmp1780 = add i32 %load_var_R_ESP_321779, -4
  store i32 %-_tmp1780, i32* @R_ESP_32, align 4
  %load_address1782 = inttoptr i32 %-_tmp1780 to i32*
  store i32 134514275, i32* %load_address1782, align 4
  br label %BB_0

BB_208:                                           ; preds = %BB_207, %switch
  %load_var_R_EBX_321777 = load i32* @R_EBX_32, align 4
  %"+_tmp1778" = add i32 %load_var_R_EBX_321777, -244
  store i32 %"+_tmp1778", i32* @R_ESI_32, align 4
  br label %BB_209

BB_207:                                           ; preds = %BB_206, %switch
  %load_var_R_ESP_321773 = load i32* @R_ESP_32, align 4
  %"+_tmp1774" = add i32 %load_var_R_ESP_321773, 48
  %load_address1775 = inttoptr i32 %"+_tmp1774" to i32*
  %load1776 = load i32* %load_address1775, align 4
  store i32 %load1776, i32* @R_EBP_32, align 4
  br label %BB_208

BB_206:                                           ; preds = %BB_205, %switch
  %load_var_R_ESP_321740 = load i32* @R_ESP_32, align 4
  %-_tmp1742 = add i32 %load_var_R_ESP_321740, -28
  store i32 %-_tmp1742, i32* @R_ESP_32, align 4
  %"<_tmp1743" = icmp ult i32 %load_var_R_ESP_321740, 28
  store i1 %"<_tmp1743", i1* @R_CF, align 1
  %15 = sub i32 27, %load_var_R_ESP_321740
  %"&_tmp1749" = and i32 %15, %load_var_R_ESP_321740
  %cast_high1751 = icmp slt i32 %"&_tmp1749", 0
  store i1 %cast_high1751, i1* @R_OF, align 1
  %load_var_R_ESP_321752 = load i32* @R_ESP_32, align 4
  %"^_tmp1754" = xor i32 %load_var_R_ESP_321752, %load_var_R_ESP_321740
  %"^_tmp1755" = and i32 %"^_tmp1754", 16
  %"==_tmp1757" = icmp eq i32 %"^_tmp1755", 0
  store i1 %"==_tmp1757", i1* @R_AF, align 1
  %">>_tmp1759" = lshr i32 %load_var_R_ESP_321752, 4
  %"^_tmp1761" = xor i32 %">>_tmp1759", %load_var_R_ESP_321752
  %">>_tmp1762" = lshr i32 %"^_tmp1761", 2
  %"^_tmp1763" = xor i32 %">>_tmp1762", %"^_tmp1761"
  %">>_tmp1764" = lshr i32 %"^_tmp1763", 1
  %"^_tmp1765" = xor i32 %">>_tmp1764", %"^_tmp1763"
  %16 = and i32 %"^_tmp1765", 1
  %bwnot1767 = icmp eq i32 %16, 0
  store i1 %bwnot1767, i1* @R_PF, align 1
  %load_var_R_ESP_321768 = load i32* @R_ESP_32, align 4
  %cast_high1770 = icmp slt i32 %load_var_R_ESP_321768, 0
  store i1 %cast_high1770, i1* @R_SF, align 1
  %"==_tmp1772" = icmp eq i32 %load_var_R_ESP_321768, 0
  store i1 %"==_tmp1772", i1* @R_ZF, align 1
  br label %BB_207

BB_205:                                           ; preds = %switch
  %load_var_R_EBX_321703 = load i32* @R_EBX_32, align 4
  %uadd2232 = call { i32, i1 } @llvm.uadd.with.overflow.i32(i32 %load_var_R_EBX_321703, i32 6581)
  %17 = extractvalue { i32, i1 } %uadd2232, 0
  store i32 %17, i32* @R_EBX_32, align 4
  %"<_tmp1707" = extractvalue { i32, i1 } %uadd2232, 1
  store i1 %"<_tmp1707", i1* @R_CF, align 1
  %"^_tmp1711" = xor i32 %load_var_R_EBX_321703, -2147483648
  %"^_tmp1714" = xor i32 %17, %load_var_R_EBX_321703
  %"&_tmp1715" = and i32 %"^_tmp1714", %"^_tmp1711"
  %cast_high1717 = icmp slt i32 %"&_tmp1715", 0
  store i1 %cast_high1717, i1* @R_OF, align 1
  %load_var_R_EBX_321718 = load i32* @R_EBX_32, align 4
  %"^_tmp1720" = xor i32 %load_var_R_EBX_321718, %load_var_R_EBX_321703
  %"^_tmp1722" = and i32 %"^_tmp1720", 16
  %"==_tmp1724" = icmp eq i32 %"^_tmp1722", 0
  store i1 %"==_tmp1724", i1* @R_AF, align 1
  %">>_tmp1726" = lshr i32 %load_var_R_EBX_321718, 4
  %"^_tmp1728" = xor i32 %">>_tmp1726", %load_var_R_EBX_321718
  %">>_tmp1729" = lshr i32 %"^_tmp1728", 2
  %"^_tmp1730" = xor i32 %">>_tmp1729", %"^_tmp1728"
  %">>_tmp1731" = lshr i32 %"^_tmp1730", 1
  %"^_tmp1732" = xor i32 %">>_tmp1731", %"^_tmp1730"
  %18 = and i32 %"^_tmp1732", 1
  %bwnot1734 = icmp eq i32 %18, 0
  store i1 %bwnot1734, i1* @R_PF, align 1
  %load_var_R_EBX_321735 = load i32* @R_EBX_32, align 4
  %cast_high1737 = icmp slt i32 %load_var_R_EBX_321735, 0
  store i1 %cast_high1737, i1* @R_SF, align 1
  %"==_tmp1739" = icmp eq i32 %load_var_R_EBX_321735, 0
  store i1 %"==_tmp1739", i1* @R_ZF, align 1
  br label %BB_206

BB_204:                                           ; preds = %BB_203, %switch
  %load_var_R_ESP_321699 = load i32* @R_ESP_32, align 4
  %-_tmp1700 = add i32 %load_var_R_ESP_321699, -4
  store i32 %-_tmp1700, i32* @R_ESP_32, align 4
  %load_address1702 = inttoptr i32 %-_tmp1700 to i32*
  store i32 134514251, i32* %load_address1702, align 4
  br label %BB_44

BB_203:                                           ; preds = %BB_202, %switch
  %load_var_R_EBX_321694 = load i32* @R_EBX_32, align 4
  %load_var_R_ESP_321695 = load i32* @R_ESP_32, align 4
  %-_tmp1696 = add i32 %load_var_R_ESP_321695, -4
  store i32 %-_tmp1696, i32* @R_ESP_32, align 4
  %load_address1698 = inttoptr i32 %-_tmp1696 to i32*
  store i32 %load_var_R_EBX_321694, i32* %load_address1698, align 4
  br label %BB_204

BB_202:                                           ; preds = %BB_201, %switch
  %load_var_R_ESI_321689 = load i32* @R_ESI_32, align 4
  %load_var_R_ESP_321690 = load i32* @R_ESP_32, align 4
  %-_tmp1691 = add i32 %load_var_R_ESP_321690, -4
  store i32 %-_tmp1691, i32* @R_ESP_32, align 4
  %load_address1693 = inttoptr i32 %-_tmp1691 to i32*
  store i32 %load_var_R_ESI_321689, i32* %load_address1693, align 4
  br label %BB_203

BB_201:                                           ; preds = %BB_200, %switch
  store i32 0, i32* @R_EDI_32, align 4
  store i1 true, i1* @R_ZF, align 1
  store i1 true, i1* @R_PF, align 1
  store i1 false, i1* @R_OF, align 1
  store i1 false, i1* @R_CF, align 1
  store i1 false, i1* @R_SF, align 1
  br label %BB_202

BB_200:                                           ; preds = %BB_199, %switch
  %load_var_R_EDI_321684 = load i32* @R_EDI_32, align 4
  %load_var_R_ESP_321685 = load i32* @R_ESP_32, align 4
  %-_tmp1686 = add i32 %load_var_R_ESP_321685, -4
  store i32 %-_tmp1686, i32* @R_ESP_32, align 4
  %load_address1688 = inttoptr i32 %-_tmp1686 to i32*
  store i32 %load_var_R_EDI_321684, i32* %load_address1688, align 4
  br label %BB_201

BB_199:                                           ; preds = %switch, %switch, %switch
  %load_var_R_EBP_321679 = load i32* @R_EBP_32, align 4
  %load_var_R_ESP_321680 = load i32* @R_ESP_32, align 4
  %-_tmp1681 = add i32 %load_var_R_ESP_321680, -4
  store i32 %-_tmp1681, i32* @R_ESP_32, align 4
  %load_address1683 = inttoptr i32 %-_tmp1681 to i32*
  store i32 %load_var_R_EBP_321679, i32* %load_address1683, align 4
  br label %BB_200

BB_198:                                           ; preds = %BB_197, %switch.BB_198_crit_edge
  %load_var_R_ESP_321673 = phi i32 [ %load_var_R_ESP_321673.pre, %switch.BB_198_crit_edge ], [ %"+_tmp1672", %BB_197 ]
  %load_address1674 = inttoptr i32 %load_var_R_ESP_321673 to i32*
  %load1675 = load i32* %load_address1674, align 4
  %"+_tmp1677" = add i32 %load_var_R_ESP_321673, 4
  store i32 %"+_tmp1677", i32* @R_ESP_32, align 4
  br label %switch

BB_197:                                           ; preds = %BB_195, %switch
  %load_var_R_EBP_321667 = load i32* @R_EBP_32, align 4
  store i32 %load_var_R_EBP_321667, i32* @R_ESP_32, align 4
  %load_address1669 = inttoptr i32 %load_var_R_EBP_321667 to i32*
  %load1670 = load i32* %load_address1669, align 4
  store i32 %load1670, i32* @R_EBP_32, align 4
  %"+_tmp1672" = add i32 %load_var_R_EBP_321667, 4
  store i32 %"+_tmp1672", i32* @R_ESP_32, align 4
  br label %BB_198

BB_196:                                           ; preds = %BB_195, %switch
  %load_var_R_ESP_321663 = load i32* @R_ESP_32, align 4
  %-_tmp1664 = add i32 %load_var_R_ESP_321663, -4
  store i32 %-_tmp1664, i32* @R_ESP_32, align 4
  %load_address1666 = inttoptr i32 %-_tmp1664 to i32*
  store i32 134514235, i32* %load_address1666, align 4
  br label %BB_15

BB_195:                                           ; preds = %BB_194, %switch
  %load_var_R_ZF1662 = load i1* @R_ZF, align 1
  br i1 %load_var_R_ZF1662, label %BB_197, label %BB_196

BB_194:                                           ; preds = %BB_193, %switch.BB_194_crit_edge
  %load_var_R_EDX_321641 = phi i32 [ %load_var_R_EDX_321641.pre, %switch.BB_194_crit_edge ], [ %load1640, %BB_193 ]
  %load_var_R_GS_BASE_321642 = load i32* @R_GS_BASE_32, align 4
  %"+_tmp1643" = add i32 %load_var_R_GS_BASE_321642, 20
  %load_address1644 = inttoptr i32 %"+_tmp1643" to i32*
  %load1645 = load i32* %load_address1644, align 4
  %"^_tmp1646" = xor i32 %load1645, %load_var_R_EDX_321641
  store i32 %"^_tmp1646", i32* @R_EDX_32, align 4
  store i1 false, i1* @R_OF, align 1
  store i1 false, i1* @R_CF, align 1
  %">>_tmp1648" = lshr i32 %"^_tmp1646", 4
  %"^_tmp1650" = xor i32 %">>_tmp1648", %"^_tmp1646"
  %">>_tmp1651" = lshr i32 %"^_tmp1650", 2
  %"^_tmp1652" = xor i32 %">>_tmp1651", %"^_tmp1650"
  %">>_tmp1653" = lshr i32 %"^_tmp1652", 1
  %"^_tmp1654" = xor i32 %">>_tmp1653", %"^_tmp1652"
  %19 = and i32 %"^_tmp1654", 1
  %bwnot1656 = icmp eq i32 %19, 0
  store i1 %bwnot1656, i1* @R_PF, align 1
  %load_var_R_EDX_321657 = load i32* @R_EDX_32, align 4
  %cast_high1659 = icmp slt i32 %load_var_R_EDX_321657, 0
  store i1 %cast_high1659, i1* @R_SF, align 1
  %"==_tmp1661" = icmp eq i32 %load_var_R_EDX_321657, 0
  store i1 %"==_tmp1661", i1* @R_ZF, align 1
  br label %BB_195

BB_193:                                           ; preds = %BB_192, %switch
  %load_var_R_ESP_321637 = load i32* @R_ESP_32, align 4
  %"+_tmp1638" = add i32 %load_var_R_ESP_321637, 76
  %load_address1639 = inttoptr i32 %"+_tmp1638" to i32*
  %load1640 = load i32* %load_address1639, align 4
  store i32 %load1640, i32* @R_EDX_32, align 4
  br label %BB_194

BB_192:                                           ; preds = %switch
  store i32 0, i32* @R_EAX_32, align 4
  br label %BB_193

BB_191:                                           ; preds = %BB_190, %switch
  %load_var_R_ESP_321633 = load i32* @R_ESP_32, align 4
  %-_tmp1634 = add i32 %load_var_R_ESP_321633, -4
  store i32 %-_tmp1634, i32* @R_ESP_32, align 4
  %load_address1636 = inttoptr i32 %-_tmp1634 to i32*
  store i32 134514212, i32* %load_address1636, align 4
  br label %BB_24

BB_190:                                           ; preds = %BB_189, %switch.BB_190_crit_edge
  %load_var_R_EAX_321632 = phi i32 [ %load_var_R_EAX_321632.pre, %switch.BB_190_crit_edge ], [ %"+_tmp1629", %BB_189 ]
  %load_var_R_ESP_321630 = load i32* @R_ESP_32, align 4
  %load_address1631 = inttoptr i32 %load_var_R_ESP_321630 to i32*
  store i32 %load_var_R_EAX_321632, i32* %load_address1631, align 4
  br label %BB_191

BB_189:                                           ; preds = %BB_188, %switch
  %load_var_R_ESP_321628 = load i32* @R_ESP_32, align 4
  %"+_tmp1629" = add i32 %load_var_R_ESP_321628, 64
  store i32 %"+_tmp1629", i32* @R_EAX_32, align 4
  br label %BB_190

BB_188:                                           ; preds = %switch
  %load_var_R_ESP_321625 = load i32* @R_ESP_32, align 4
  %"+_tmp1626" = add i32 %load_var_R_ESP_321625, 4
  %load_address1627 = inttoptr i32 %"+_tmp1626" to i32*
  store i32 134514390, i32* %load_address1627, align 4
  br label %BB_189

BB_187:                                           ; preds = %BB_186, %switch
  %load_var_R_ESP_321621 = load i32* @R_ESP_32, align 4
  %-_tmp1622 = add i32 %load_var_R_ESP_321621, -4
  store i32 %-_tmp1622, i32* @R_ESP_32, align 4
  %load_address1624 = inttoptr i32 %-_tmp1622 to i32*
  store i32 134514192, i32* %load_address1624, align 4
  br label %BB_24

BB_186:                                           ; preds = %BB_185, %switch.BB_186_crit_edge
  %load_var_R_EAX_321620 = phi i32 [ %load_var_R_EAX_321620.pre, %switch.BB_186_crit_edge ], [ %load1617, %BB_185 ]
  %load_var_R_ESP_321618 = load i32* @R_ESP_32, align 4
  %load_address1619 = inttoptr i32 %load_var_R_ESP_321618 to i32*
  store i32 %load_var_R_EAX_321620, i32* %load_address1619, align 4
  br label %BB_187

BB_185:                                           ; preds = %BB_184, %switch
  %load_var_R_ESP_321614 = load i32* @R_ESP_32, align 4
  %"+_tmp1615" = add i32 %load_var_R_ESP_321614, 32
  %load_address1616 = inttoptr i32 %"+_tmp1615" to i32*
  %load1617 = load i32* %load_address1616, align 4
  store i32 %load1617, i32* @R_EAX_32, align 4
  br label %BB_186

BB_184:                                           ; preds = %BB_183, %switch
  %load_var_R_ESP_321611 = load i32* @R_ESP_32, align 4
  %"+_tmp1612" = add i32 %load_var_R_ESP_321611, 4
  %load_address1613 = inttoptr i32 %"+_tmp1612" to i32*
  store i32 134514390, i32* %load_address1613, align 4
  br label %BB_185

BB_183:                                           ; preds = %BB_182, %switch.BB_183_crit_edge
  %load_var_R_EAX_321610 = phi i32 [ %load_var_R_EAX_321610.pre, %switch.BB_183_crit_edge ], [ %load1606, %BB_182 ]
  %load_var_R_ESP_321607 = load i32* @R_ESP_32, align 4
  %"+_tmp1608" = add i32 %load_var_R_ESP_321607, 48
  %load_address1609 = inttoptr i32 %"+_tmp1608" to i32*
  store i32 %load_var_R_EAX_321610, i32* %load_address1609, align 4
  br label %BB_184

BB_182:                                           ; preds = %BB_181, %switch
  %load_var_R_ESP_321603 = load i32* @R_ESP_32, align 4
  %"+_tmp1604" = add i32 %load_var_R_ESP_321603, 32
  %load_address1605 = inttoptr i32 %"+_tmp1604" to i32*
  %load1606 = load i32* %load_address1605, align 4
  store i32 %load1606, i32* @R_EAX_32, align 4
  br label %BB_183

BB_181:                                           ; preds = %BB_180, %switch.BB_181_crit_edge
  %load_var_R_EAX_321602 = phi i32 [ %load_var_R_EAX_321602.pre, %switch.BB_181_crit_edge ], [ %load1598, %BB_180 ]
  %load_var_R_ESP_321599 = load i32* @R_ESP_32, align 4
  %"+_tmp1600" = add i32 %load_var_R_ESP_321599, 44
  %load_address1601 = inttoptr i32 %"+_tmp1600" to i32*
  store i32 %load_var_R_EAX_321602, i32* %load_address1601, align 4
  br label %BB_182

BB_180:                                           ; preds = %BB_179, %switch.BB_180_crit_edge
  %load_var_R_EAX_321595 = phi i32 [ %load_var_R_EAX_321595.pre, %switch.BB_180_crit_edge ], [ %load1594, %BB_179 ]
  %"+_tmp1596" = add i32 %load_var_R_EAX_321595, 4
  %load_address1597 = inttoptr i32 %"+_tmp1596" to i32*
  %load1598 = load i32* %load_address1597, align 4
  store i32 %load1598, i32* @R_EAX_32, align 4
  br label %BB_181

BB_179:                                           ; preds = %BB_178, %switch
  %load_var_R_ESP_321591 = load i32* @R_ESP_32, align 4
  %"+_tmp1592" = add i32 %load_var_R_ESP_321591, 12
  %load_address1593 = inttoptr i32 %"+_tmp1592" to i32*
  %load1594 = load i32* %load_address1593, align 4
  store i32 %load1594, i32* @R_EAX_32, align 4
  br label %BB_180

BB_178:                                           ; preds = %switch
  %load_var_R_ESP_321587 = load i32* @R_ESP_32, align 4
  %"+_tmp1588" = add i32 %load_var_R_ESP_321587, 40
  %load_address1589 = inttoptr i32 %"+_tmp1588" to i32*
  %load_var_R_EAX_321590 = load i32* @R_EAX_32, align 4
  store i32 %load_var_R_EAX_321590, i32* %load_address1589, align 4
  br label %BB_179

BB_177:                                           ; preds = %BB_176, %switch
  %load_var_R_ESP_321583 = load i32* @R_ESP_32, align 4
  %-_tmp1584 = add i32 %load_var_R_ESP_321583, -4
  store i32 %-_tmp1584, i32* @R_ESP_32, align 4
  %load_address1586 = inttoptr i32 %-_tmp1584 to i32*
  store i32 134514149, i32* %load_address1586, align 4
  br label %BB_128

BB_176:                                           ; preds = %BB_175, %switch.BB_176_crit_edge
  %load_var_R_EAX_321582 = phi i32 [ %load_var_R_EAX_321582.pre, %switch.BB_176_crit_edge ], [ %load1579, %BB_175 ]
  %load_var_R_ESP_321580 = load i32* @R_ESP_32, align 4
  %load_address1581 = inttoptr i32 %load_var_R_ESP_321580 to i32*
  store i32 %load_var_R_EAX_321582, i32* %load_address1581, align 4
  br label %BB_177

BB_175:                                           ; preds = %BB_174, %switch
  %load_var_R_ESP_321576 = load i32* @R_ESP_32, align 4
  %"+_tmp1577" = add i32 %load_var_R_ESP_321576, 36
  %load_address1578 = inttoptr i32 %"+_tmp1577" to i32*
  %load1579 = load i32* %load_address1578, align 4
  store i32 %load1579, i32* @R_EAX_32, align 4
  br label %BB_176

BB_174:                                           ; preds = %switch
  %load_var_R_ESP_321572 = load i32* @R_ESP_32, align 4
  %"+_tmp1573" = add i32 %load_var_R_ESP_321572, 36
  %load_address1574 = inttoptr i32 %"+_tmp1573" to i32*
  %load_var_R_EAX_321575 = load i32* @R_EAX_32, align 4
  store i32 %load_var_R_EAX_321575, i32* %load_address1574, align 4
  br label %BB_175

BB_173:                                           ; preds = %BB_172, %switch
  %load_var_R_ESP_321568 = load i32* @R_ESP_32, align 4
  %-_tmp1569 = add i32 %load_var_R_ESP_321568, -4
  store i32 %-_tmp1569, i32* @R_ESP_32, align 4
  %load_address1571 = inttoptr i32 %-_tmp1569 to i32*
  store i32 134514133, i32* %load_address1571, align 4
  br label %BB_107

BB_172:                                           ; preds = %BB_171, %switch.BB_172_crit_edge
  %load_var_R_EAX_321567 = phi i32 [ %load_var_R_EAX_321567.pre, %switch.BB_172_crit_edge ], [ %load1564, %BB_171 ]
  %load_var_R_ESP_321565 = load i32* @R_ESP_32, align 4
  %load_address1566 = inttoptr i32 %load_var_R_ESP_321565 to i32*
  store i32 %load_var_R_EAX_321567, i32* %load_address1566, align 4
  br label %BB_173

BB_171:                                           ; preds = %switch
  %load_var_R_ESP_321561 = load i32* @R_ESP_32, align 4
  %"+_tmp1562" = add i32 %load_var_R_ESP_321561, 28
  %load_address1563 = inttoptr i32 %"+_tmp1562" to i32*
  %load1564 = load i32* %load_address1563, align 4
  store i32 %load1564, i32* @R_EAX_32, align 4
  br label %BB_172

BB_170:                                           ; preds = %BB_169, %switch
  %load_var_R_ESP_321557 = load i32* @R_ESP_32, align 4
  %-_tmp1558 = add i32 %load_var_R_ESP_321557, -4
  store i32 %-_tmp1558, i32* @R_ESP_32, align 4
  %load_address1560 = inttoptr i32 %-_tmp1558 to i32*
  store i32 134514121, i32* %load_address1560, align 4
  br label %BB_27

BB_169:                                           ; preds = %BB_168, %switch
  %load_var_R_ESP_321555 = load i32* @R_ESP_32, align 4
  %load_address1556 = inttoptr i32 %load_var_R_ESP_321555 to i32*
  store i32 134514387, i32* %load_address1556, align 4
  br label %BB_170

BB_168:                                           ; preds = %BB_167, %switch.BB_168_crit_edge
  %load_var_R_EAX_321554 = phi i32 [ %load_var_R_EAX_321554.pre, %switch.BB_168_crit_edge ], [ %"+_tmp1550", %BB_167 ]
  %load_var_R_ESP_321551 = load i32* @R_ESP_32, align 4
  %"+_tmp1552" = add i32 %load_var_R_ESP_321551, 4
  %load_address1553 = inttoptr i32 %"+_tmp1552" to i32*
  store i32 %load_var_R_EAX_321554, i32* %load_address1553, align 4
  br label %BB_169

BB_167:                                           ; preds = %switch
  %load_var_R_ESP_321549 = load i32* @R_ESP_32, align 4
  %"+_tmp1550" = add i32 %load_var_R_ESP_321549, 28
  store i32 %"+_tmp1550", i32* @R_EAX_32, align 4
  br label %BB_168

BB_166:                                           ; preds = %BB_165, %switch
  %load_var_R_ESP_321545 = load i32* @R_ESP_32, align 4
  %-_tmp1546 = add i32 %load_var_R_ESP_321545, -4
  store i32 %-_tmp1546, i32* @R_ESP_32, align 4
  %load_address1548 = inttoptr i32 %-_tmp1546 to i32*
  store i32 134514101, i32* %load_address1548, align 4
  br label %BB_27

BB_165:                                           ; preds = %BB_164, %switch
  %load_var_R_ESP_321543 = load i32* @R_ESP_32, align 4
  %load_address1544 = inttoptr i32 %load_var_R_ESP_321543 to i32*
  store i32 134514384, i32* %load_address1544, align 4
  br label %BB_166

BB_164:                                           ; preds = %BB_163, %switch.BB_164_crit_edge
  %load_var_R_EAX_321542 = phi i32 [ %load_var_R_EAX_321542.pre, %switch.BB_164_crit_edge ], [ %load1538, %BB_163 ]
  %load_var_R_ESP_321539 = load i32* @R_ESP_32, align 4
  %"+_tmp1540" = add i32 %load_var_R_ESP_321539, 4
  %load_address1541 = inttoptr i32 %"+_tmp1540" to i32*
  store i32 %load_var_R_EAX_321542, i32* %load_address1541, align 4
  br label %BB_165

BB_163:                                           ; preds = %BB_162, %switch
  %load_var_R_ESP_321535 = load i32* @R_ESP_32, align 4
  %"+_tmp1536" = add i32 %load_var_R_ESP_321535, 32
  %load_address1537 = inttoptr i32 %"+_tmp1536" to i32*
  %load1538 = load i32* %load_address1537, align 4
  store i32 %load1538, i32* @R_EAX_32, align 4
  br label %BB_164

BB_162:                                           ; preds = %BB_161, %switch
  %load_var_R_ESP_321532 = load i32* @R_ESP_32, align 4
  %"+_tmp1533" = add i32 %load_var_R_ESP_321532, 66
  %load_address1534 = inttoptr i32 %"+_tmp1533" to i8*
  store i8 99, i8* %load_address1534, align 1
  br label %BB_163

BB_161:                                           ; preds = %BB_160, %switch
  %load_var_R_ESP_321529 = load i32* @R_ESP_32, align 4
  %"+_tmp1530" = add i32 %load_var_R_ESP_321529, 65
  %load_address1531 = inttoptr i32 %"+_tmp1530" to i8*
  store i8 98, i8* %load_address1531, align 1
  br label %BB_162

BB_160:                                           ; preds = %BB_159, %switch
  %load_var_R_ESP_321526 = load i32* @R_ESP_32, align 4
  %"+_tmp1527" = add i32 %load_var_R_ESP_321526, 64
  %load_address1528 = inttoptr i32 %"+_tmp1527" to i8*
  store i8 97, i8* %load_address1528, align 1
  br label %BB_161

BB_159:                                           ; preds = %BB_158, %switch
  %load_var_R_ESP_321523 = load i32* @R_ESP_32, align 4
  %"+_tmp1524" = add i32 %load_var_R_ESP_321523, 72
  %load_address1525 = inttoptr i32 %"+_tmp1524" to i32*
  store i32 0, i32* %load_address1525, align 4
  br label %BB_160

BB_158:                                           ; preds = %BB_157, %switch
  %load_var_R_ESP_321520 = load i32* @R_ESP_32, align 4
  %"+_tmp1521" = add i32 %load_var_R_ESP_321520, 68
  %load_address1522 = inttoptr i32 %"+_tmp1521" to i32*
  store i32 0, i32* %load_address1522, align 4
  br label %BB_159

BB_157:                                           ; preds = %BB_156, %switch
  %load_var_R_ESP_321517 = load i32* @R_ESP_32, align 4
  %"+_tmp1518" = add i32 %load_var_R_ESP_321517, 64
  %load_address1519 = inttoptr i32 %"+_tmp1518" to i32*
  store i32 0, i32* %load_address1519, align 4
  br label %BB_158

BB_156:                                           ; preds = %BB_155, %switch.BB_156_crit_edge
  %load_var_R_EAX_321516 = phi i32 [ %load_var_R_EAX_321516.pre, %switch.BB_156_crit_edge ], [ %"+_tmp1512", %BB_155 ]
  %load_var_R_ESP_321513 = load i32* @R_ESP_32, align 4
  %"+_tmp1514" = add i32 %load_var_R_ESP_321513, 32
  %load_address1515 = inttoptr i32 %"+_tmp1514" to i32*
  store i32 %load_var_R_EAX_321516, i32* %load_address1515, align 4
  br label %BB_157

BB_155:                                           ; preds = %BB_154, %switch
  %load_var_R_ESP_321511 = load i32* @R_ESP_32, align 4
  %"+_tmp1512" = add i32 %load_var_R_ESP_321511, 54
  store i32 %"+_tmp1512", i32* @R_EAX_32, align 4
  br label %BB_156

BB_154:                                           ; preds = %BB_153, %switch
  store i32 0, i32* @R_EAX_32, align 4
  store i1 true, i1* @R_ZF, align 1
  store i1 true, i1* @R_PF, align 1
  store i1 false, i1* @R_OF, align 1
  store i1 false, i1* @R_CF, align 1
  store i1 false, i1* @R_SF, align 1
  br label %BB_155

BB_153:                                           ; preds = %BB_152, %switch.BB_153_crit_edge
  %load_var_R_EAX_321510 = phi i32 [ %load_var_R_EAX_321510.pre, %switch.BB_153_crit_edge ], [ %load1506, %BB_152 ]
  %load_var_R_ESP_321507 = load i32* @R_ESP_32, align 4
  %"+_tmp1508" = add i32 %load_var_R_ESP_321507, 76
  %load_address1509 = inttoptr i32 %"+_tmp1508" to i32*
  store i32 %load_var_R_EAX_321510, i32* %load_address1509, align 4
  br label %BB_154

BB_152:                                           ; preds = %BB_151, %switch
  %load_var_R_GS_BASE_32 = load i32* @R_GS_BASE_32, align 4
  %"+_tmp1504" = add i32 %load_var_R_GS_BASE_32, 20
  %load_address1505 = inttoptr i32 %"+_tmp1504" to i32*
  %load1506 = load i32* %load_address1505, align 4
  store i32 %load1506, i32* @R_EAX_32, align 4
  br label %BB_153

BB_151:                                           ; preds = %BB_150, %switch.BB_151_crit_edge
  %load_var_R_EAX_321503 = phi i32 [ %load_var_R_EAX_321503.pre, %switch.BB_151_crit_edge ], [ %load1499, %BB_150 ]
  %load_var_R_ESP_321500 = load i32* @R_ESP_32, align 4
  %"+_tmp1501" = add i32 %load_var_R_ESP_321500, 12
  %load_address1502 = inttoptr i32 %"+_tmp1501" to i32*
  store i32 %load_var_R_EAX_321503, i32* %load_address1502, align 4
  br label %BB_152

BB_150:                                           ; preds = %BB_149, %switch
  %load_var_R_EBP_321496 = load i32* @R_EBP_32, align 4
  %"+_tmp1497" = add i32 %load_var_R_EBP_321496, 12
  %load_address1498 = inttoptr i32 %"+_tmp1497" to i32*
  %load1499 = load i32* %load_address1498, align 4
  store i32 %load1499, i32* @R_EAX_32, align 4
  br label %BB_151

BB_149:                                           ; preds = %BB_148, %switch
  %load_var_R_ESP_321463 = load i32* @R_ESP_32, align 4
  %-_tmp1465 = add i32 %load_var_R_ESP_321463, -80
  store i32 %-_tmp1465, i32* @R_ESP_32, align 4
  %"<_tmp1466" = icmp ult i32 %load_var_R_ESP_321463, 80
  store i1 %"<_tmp1466", i1* @R_CF, align 1
  %20 = sub i32 79, %load_var_R_ESP_321463
  %"&_tmp1472" = and i32 %20, %load_var_R_ESP_321463
  %cast_high1474 = icmp slt i32 %"&_tmp1472", 0
  store i1 %cast_high1474, i1* @R_OF, align 1
  %load_var_R_ESP_321475 = load i32* @R_ESP_32, align 4
  %"^_tmp1477" = xor i32 %load_var_R_ESP_321475, %load_var_R_ESP_321463
  %"^_tmp1478" = and i32 %"^_tmp1477", 16
  %"==_tmp1480" = icmp eq i32 %"^_tmp1478", 0
  store i1 %"==_tmp1480", i1* @R_AF, align 1
  %">>_tmp1482" = lshr i32 %load_var_R_ESP_321475, 4
  %"^_tmp1484" = xor i32 %">>_tmp1482", %load_var_R_ESP_321475
  %">>_tmp1485" = lshr i32 %"^_tmp1484", 2
  %"^_tmp1486" = xor i32 %">>_tmp1485", %"^_tmp1484"
  %">>_tmp1487" = lshr i32 %"^_tmp1486", 1
  %"^_tmp1488" = xor i32 %">>_tmp1487", %"^_tmp1486"
  %21 = and i32 %"^_tmp1488", 1
  %bwnot1490 = icmp eq i32 %21, 0
  store i1 %bwnot1490, i1* @R_PF, align 1
  %load_var_R_ESP_321491 = load i32* @R_ESP_32, align 4
  %cast_high1493 = icmp slt i32 %load_var_R_ESP_321491, 0
  store i1 %cast_high1493, i1* @R_SF, align 1
  %"==_tmp1495" = icmp eq i32 %load_var_R_ESP_321491, 0
  store i1 %"==_tmp1495", i1* @R_ZF, align 1
  br label %BB_150

BB_148:                                           ; preds = %BB_147, %switch
  %load_var_R_ESP_321446 = load i32* @R_ESP_32, align 4
  %"&_tmp1447" = and i32 %load_var_R_ESP_321446, -16
  store i32 %"&_tmp1447", i32* @R_ESP_32, align 4
  store i1 false, i1* @R_OF, align 1
  store i1 false, i1* @R_CF, align 1
  %">>_tmp1449" = lshr i32 %load_var_R_ESP_321446, 4
  %"^_tmp1451" = xor i32 %">>_tmp1449", %"&_tmp1447"
  %">>_tmp1452" = lshr i32 %"^_tmp1451", 2
  %"^_tmp1453" = xor i32 %">>_tmp1452", %"^_tmp1451"
  %">>_tmp1454" = lshr i32 %"^_tmp1453", 1
  %"^_tmp1455" = xor i32 %">>_tmp1454", %"^_tmp1453"
  %22 = and i32 %"^_tmp1455", 1
  %bwnot1457 = icmp eq i32 %22, 0
  store i1 %bwnot1457, i1* @R_PF, align 1
  %load_var_R_ESP_321458 = load i32* @R_ESP_32, align 4
  %cast_high1460 = icmp slt i32 %load_var_R_ESP_321458, 0
  store i1 %cast_high1460, i1* @R_SF, align 1
  %"==_tmp1462" = icmp eq i32 %load_var_R_ESP_321458, 0
  store i1 %"==_tmp1462", i1* @R_ZF, align 1
  br label %BB_149

BB_147:                                           ; preds = %BB_146, %switch
  %load_var_R_ESP_321445 = load i32* @R_ESP_32, align 4
  store i32 %load_var_R_ESP_321445, i32* @R_EBP_32, align 4
  br label %BB_148

BB_146:                                           ; preds = %switch
  %load_var_R_EBP_321440 = load i32* @R_EBP_32, align 4
  %load_var_R_ESP_321441 = load i32* @R_ESP_32, align 4
  %-_tmp1442 = add i32 %load_var_R_ESP_321441, -4
  store i32 %-_tmp1442, i32* @R_ESP_32, align 4
  %load_address1444 = inttoptr i32 %-_tmp1442 to i32*
  store i32 %load_var_R_EBP_321440, i32* %load_address1444, align 4
  br label %BB_147

BB_145:                                           ; preds = %BB_144, %switch.BB_145_crit_edge
  %load_var_R_ESP_321434 = phi i32 [ %load_var_R_ESP_321434.pre, %switch.BB_145_crit_edge ], [ %"+_tmp1433", %BB_144 ]
  %load_address1435 = inttoptr i32 %load_var_R_ESP_321434 to i32*
  %load1436 = load i32* %load_address1435, align 4
  %"+_tmp1438" = add i32 %load_var_R_ESP_321434, 4
  store i32 %"+_tmp1438", i32* @R_ESP_32, align 4
  br label %switch

BB_144:                                           ; preds = %BB_143, %switch
  %load_var_R_EBP_321428 = load i32* @R_EBP_32, align 4
  store i32 %load_var_R_EBP_321428, i32* @R_ESP_32, align 4
  %load_address1430 = inttoptr i32 %load_var_R_EBP_321428 to i32*
  %load1431 = load i32* %load_address1430, align 4
  store i32 %load1431, i32* @R_EBP_32, align 4
  %"+_tmp1433" = add i32 %load_var_R_EBP_321428, 4
  store i32 %"+_tmp1433", i32* @R_ESP_32, align 4
  br label %BB_145

BB_143:                                           ; preds = %BB_138, %BB_142, %switch, %switch
  %load_var_R_EBP_321424 = load i32* @R_EBP_32, align 4
  %"+_tmp1425" = add i32 %load_var_R_EBP_321424, -8
  %load_address1426 = inttoptr i32 %"+_tmp1425" to i32*
  %load1427 = load i32* %load_address1426, align 4
  store i32 %load1427, i32* @R_EAX_32, align 4
  br label %BB_144

BB_142:                                           ; preds = %switch
  %load_var_R_EBP_321420 = load i32* @R_EBP_32, align 4
  %"+_tmp1421" = add i32 %load_var_R_EBP_321420, -8
  %load_address1422 = inttoptr i32 %"+_tmp1421" to i32*
  %load_var_R_EAX_321423 = load i32* @R_EAX_32, align 4
  store i32 %load_var_R_EAX_321423, i32* %load_address1422, align 4
  br label %BB_143

BB_141:                                           ; preds = %BB_140, %switch
  %load_var_R_ESP_321416 = load i32* @R_ESP_32, align 4
  %-_tmp1417 = add i32 %load_var_R_ESP_321416, -4
  store i32 %-_tmp1417, i32* @R_ESP_32, align 4
  %load_address1419 = inttoptr i32 %-_tmp1417 to i32*
  store i32 134513998, i32* %load_address1419, align 4
  br label %BB_123

BB_140:                                           ; preds = %BB_134, %switch
  %load_var_R_ESP_321414 = load i32* @R_ESP_32, align 4
  %load_address1415 = inttoptr i32 %load_var_R_ESP_321414 to i32*
  store i32 5, i32* %load_address1415, align 4
  br label %BB_141

BB_138:                                           ; preds = %switch
  %load_var_R_EBP_321410 = load i32* @R_EBP_32, align 4
  %"+_tmp1411" = add i32 %load_var_R_EBP_321410, -8
  %load_address1412 = inttoptr i32 %"+_tmp1411" to i32*
  %load_var_R_EAX_321413 = load i32* @R_EAX_32, align 4
  store i32 %load_var_R_EAX_321413, i32* %load_address1412, align 4
  br label %BB_143

BB_137:                                           ; preds = %BB_136, %switch
  %load_var_R_ESP_321406 = load i32* @R_ESP_32, align 4
  %-_tmp1407 = add i32 %load_var_R_ESP_321406, -4
  store i32 %-_tmp1407, i32* @R_ESP_32, align 4
  %load_address1409 = inttoptr i32 %-_tmp1407 to i32*
  store i32 134513981, i32* %load_address1409, align 4
  br label %BB_123

BB_136:                                           ; preds = %BB_135, %switch.BB_136_crit_edge
  %load_var_R_EAX_321405 = phi i32 [ %load_var_R_EAX_321405.pre, %switch.BB_136_crit_edge ], [ %load1402, %BB_135 ]
  %load_var_R_ESP_321403 = load i32* @R_ESP_32, align 4
  %load_address1404 = inttoptr i32 %load_var_R_ESP_321403 to i32*
  store i32 %load_var_R_EAX_321405, i32* %load_address1404, align 4
  br label %BB_137

BB_135:                                           ; preds = %BB_134, %switch
  %load_var_R_EBP_321399 = load i32* @R_EBP_32, align 4
  %"+_tmp1400" = add i32 %load_var_R_EBP_321399, 8
  %load_address1401 = inttoptr i32 %"+_tmp1400" to i32*
  %load1402 = load i32* %load_address1401, align 4
  store i32 %load1402, i32* @R_EAX_32, align 4
  br label %BB_136

BB_134:                                           ; preds = %BB_133, %switch
  %load_var_R_ZF1394 = load i1* @R_ZF, align 1
  %load_var_R_SF1395 = load i1* @R_SF, align 1
  %load_var_R_OF1396 = load i1* @R_OF, align 1
  %"^_tmp1397" = xor i1 %load_var_R_SF1395, %load_var_R_OF1396
  %"|_tmp1398" = or i1 %load_var_R_ZF1394, %"^_tmp1397"
  br i1 %"|_tmp1398", label %BB_140, label %BB_135

BB_133:                                           ; preds = %BB_132, %switch
  %load_var_R_EBP_321347 = load i32* @R_EBP_32, align 4
  %"+_tmp1348" = add i32 %load_var_R_EBP_321347, -4
  %load_address1349 = inttoptr i32 %"+_tmp1348" to i32*
  %load1350 = load i32* %load_address1349, align 4
  %-_tmp1351 = add i32 %load1350, -5
  %"<_tmp1356" = icmp ult i32 %load1350, 5
  store i1 %"<_tmp1356", i1* @R_CF, align 1
  %load_var_R_EBP_321357 = load i32* @R_EBP_32, align 4
  %"+_tmp1358" = add i32 %load_var_R_EBP_321357, -4
  %load_address1359 = inttoptr i32 %"+_tmp1358" to i32*
  %load1360 = load i32* %load_address1359, align 4
  %23 = sub i32 4, %load1350
  %"&_tmp1367" = and i32 %load1360, %23
  %cast_high1369 = icmp slt i32 %"&_tmp1367", 0
  store i1 %cast_high1369, i1* @R_OF, align 1
  %load_var_R_EBP_321371 = load i32* @R_EBP_32, align 4
  %"+_tmp1372" = add i32 %load_var_R_EBP_321371, -4
  %load_address1373 = inttoptr i32 %"+_tmp1372" to i32*
  %load1374 = load i32* %load_address1373, align 4
  %"^_tmp1375" = xor i32 %load1374, %-_tmp1351
  %"&_tmp1377" = and i32 %"^_tmp1375", 16
  %"==_tmp1378" = icmp ne i32 %"&_tmp1377", 0
  store i1 %"==_tmp1378", i1* @R_AF, align 1
  %">>_tmp1380" = lshr i32 %-_tmp1351, 4
  %"^_tmp1382" = xor i32 %">>_tmp1380", %-_tmp1351
  %">>_tmp1383" = lshr i32 %"^_tmp1382", 2
  %"^_tmp1384" = xor i32 %">>_tmp1383", %"^_tmp1382"
  %">>_tmp1385" = lshr i32 %"^_tmp1384", 1
  %"^_tmp1386" = xor i32 %">>_tmp1385", %"^_tmp1384"
  %24 = and i32 %"^_tmp1386", 1
  %bwnot1388 = icmp eq i32 %24, 0
  store i1 %bwnot1388, i1* @R_PF, align 1
  %cast_high1391 = icmp slt i32 %-_tmp1351, 0
  store i1 %cast_high1391, i1* @R_SF, align 1
  %"==_tmp1393" = icmp eq i32 %-_tmp1351, 0
  store i1 %"==_tmp1393", i1* @R_ZF, align 1
  br label %BB_134

BB_132:                                           ; preds = %BB_131, %switch
  %load_var_R_EBP_321344 = load i32* @R_EBP_32, align 4
  %"+_tmp1345" = add i32 %load_var_R_EBP_321344, -4
  %load_address1346 = inttoptr i32 %"+_tmp1345" to i32*
  store i32 20, i32* %load_address1346, align 4
  br label %BB_133

BB_131:                                           ; preds = %BB_130, %switch
  %load_var_R_EBP_321341 = load i32* @R_EBP_32, align 4
  %"+_tmp1342" = add i32 %load_var_R_EBP_321341, -8
  %load_address1343 = inttoptr i32 %"+_tmp1342" to i32*
  store i32 0, i32* %load_address1343, align 4
  br label %BB_132

BB_130:                                           ; preds = %BB_129, %switch
  %load_var_R_ESP_321308 = load i32* @R_ESP_32, align 4
  %-_tmp1310 = add i32 %load_var_R_ESP_321308, -20
  store i32 %-_tmp1310, i32* @R_ESP_32, align 4
  %"<_tmp1311" = icmp ult i32 %load_var_R_ESP_321308, 20
  store i1 %"<_tmp1311", i1* @R_CF, align 1
  %25 = sub i32 19, %load_var_R_ESP_321308
  %"&_tmp1317" = and i32 %25, %load_var_R_ESP_321308
  %cast_high1319 = icmp slt i32 %"&_tmp1317", 0
  store i1 %cast_high1319, i1* @R_OF, align 1
  %load_var_R_ESP_321320 = load i32* @R_ESP_32, align 4
  %"^_tmp1322" = xor i32 %load_var_R_ESP_321320, %load_var_R_ESP_321308
  %"^_tmp1323" = and i32 %"^_tmp1322", 16
  %"==_tmp1325" = icmp eq i32 %"^_tmp1323", 0
  store i1 %"==_tmp1325", i1* @R_AF, align 1
  %">>_tmp1327" = lshr i32 %load_var_R_ESP_321320, 4
  %"^_tmp1329" = xor i32 %">>_tmp1327", %load_var_R_ESP_321320
  %">>_tmp1330" = lshr i32 %"^_tmp1329", 2
  %"^_tmp1331" = xor i32 %">>_tmp1330", %"^_tmp1329"
  %">>_tmp1332" = lshr i32 %"^_tmp1331", 1
  %"^_tmp1333" = xor i32 %">>_tmp1332", %"^_tmp1331"
  %26 = and i32 %"^_tmp1333", 1
  %bwnot1335 = icmp eq i32 %26, 0
  store i1 %bwnot1335, i1* @R_PF, align 1
  %load_var_R_ESP_321336 = load i32* @R_ESP_32, align 4
  %cast_high1338 = icmp slt i32 %load_var_R_ESP_321336, 0
  store i1 %cast_high1338, i1* @R_SF, align 1
  %"==_tmp1340" = icmp eq i32 %load_var_R_ESP_321336, 0
  store i1 %"==_tmp1340", i1* @R_ZF, align 1
  br label %BB_131

BB_129:                                           ; preds = %BB_128, %switch
  %load_var_R_ESP_321307 = load i32* @R_ESP_32, align 4
  store i32 %load_var_R_ESP_321307, i32* @R_EBP_32, align 4
  br label %BB_130

BB_128:                                           ; preds = %BB_177, %switch
  %load_var_R_EBP_321302 = load i32* @R_EBP_32, align 4
  %load_var_R_ESP_321303 = load i32* @R_ESP_32, align 4
  %-_tmp1304 = add i32 %load_var_R_ESP_321303, -4
  store i32 %-_tmp1304, i32* @R_ESP_32, align 4
  %load_address1306 = inttoptr i32 %-_tmp1304 to i32*
  store i32 %load_var_R_EBP_321302, i32* %load_address1306, align 4
  br label %BB_129

BB_127:                                           ; preds = %BB_126, %switch.BB_127_crit_edge
  %load_var_R_ESP_321296 = phi i32 [ %load_var_R_ESP_321296.pre, %switch.BB_127_crit_edge ], [ %"+_tmp1295", %BB_126 ]
  %load_address1297 = inttoptr i32 %load_var_R_ESP_321296 to i32*
  %load1298 = load i32* %load_address1297, align 4
  %"+_tmp1300" = add i32 %load_var_R_ESP_321296, 4
  store i32 %"+_tmp1300", i32* @R_ESP_32, align 4
  br label %switch

BB_126:                                           ; preds = %BB_125, %switch
  %load_var_R_ESP_321291 = load i32* @R_ESP_32, align 4
  %load_address1292 = inttoptr i32 %load_var_R_ESP_321291 to i32*
  %load1293 = load i32* %load_address1292, align 4
  store i32 %load1293, i32* @R_EBP_32, align 4
  %"+_tmp1295" = add i32 %load_var_R_ESP_321291, 4
  store i32 %"+_tmp1295", i32* @R_ESP_32, align 4
  br label %BB_127

BB_125:                                           ; preds = %BB_124, %switch.BB_125_crit_edge
  %load_var_R_EBP_321287 = phi i32 [ %load_var_R_EBP_321287.pre, %switch.BB_125_crit_edge ], [ %load_var_R_ESP_321286, %BB_124 ]
  %"+_tmp1288" = add i32 %load_var_R_EBP_321287, 8
  %load_address1289 = inttoptr i32 %"+_tmp1288" to i32*
  %load1290 = load i32* %load_address1289, align 4
  store i32 %load1290, i32* @R_EAX_32, align 4
  br label %BB_126

BB_124:                                           ; preds = %BB_123, %switch
  %load_var_R_ESP_321286 = load i32* @R_ESP_32, align 4
  store i32 %load_var_R_ESP_321286, i32* @R_EBP_32, align 4
  br label %BB_125

BB_123:                                           ; preds = %BB_137, %BB_141, %switch
  %load_var_R_EBP_321281 = load i32* @R_EBP_32, align 4
  %load_var_R_ESP_321282 = load i32* @R_ESP_32, align 4
  %-_tmp1283 = add i32 %load_var_R_ESP_321282, -4
  store i32 %-_tmp1283, i32* @R_ESP_32, align 4
  %load_address1285 = inttoptr i32 %-_tmp1283 to i32*
  store i32 %load_var_R_EBP_321281, i32* %load_address1285, align 4
  br label %BB_124

BB_122:                                           ; preds = %BB_121, %switch.BB_122_crit_edge
  %load_var_R_ESP_321275 = phi i32 [ %load_var_R_ESP_321275.pre, %switch.BB_122_crit_edge ], [ %"+_tmp1274", %BB_121 ]
  %load_address1276 = inttoptr i32 %load_var_R_ESP_321275 to i32*
  %load1277 = load i32* %load_address1276, align 4
  %"+_tmp1279" = add i32 %load_var_R_ESP_321275, 4
  store i32 %"+_tmp1279", i32* @R_ESP_32, align 4
  br label %switch

BB_121:                                           ; preds = %BB_120, %switch
  %load_var_R_EBP_321269 = load i32* @R_EBP_32, align 4
  store i32 %load_var_R_EBP_321269, i32* @R_ESP_32, align 4
  %load_address1271 = inttoptr i32 %load_var_R_EBP_321269 to i32*
  %load1272 = load i32* %load_address1271, align 4
  store i32 %load1272, i32* @R_EBP_32, align 4
  %"+_tmp1274" = add i32 %load_var_R_EBP_321269, 4
  store i32 %"+_tmp1274", i32* @R_ESP_32, align 4
  br label %BB_122

BB_120:                                           ; preds = %BB_119, %switch.BB_120_crit_edge
  %load_var_R_EDX_321232 = phi i32 [ %load_var_R_EDX_321232.pre, %switch.BB_120_crit_edge ], [ %load1230, %BB_119 ]
  %load_var_R_EAX_321231 = load i32* @R_EAX_32, align 4
  %uadd2233 = call { i32, i1 } @llvm.uadd.with.overflow.i32(i32 %load_var_R_EAX_321231, i32 %load_var_R_EDX_321232)
  %27 = extractvalue { i32, i1 } %uadd2233, 0
  store i32 %27, i32* @R_EAX_32, align 4
  %"<_tmp1236" = extractvalue { i32, i1 } %uadd2233, 1
  store i1 %"<_tmp1236", i1* @R_CF, align 1
  %bwnot1239 = xor i32 %load_var_R_EDX_321232, -2147483648
  %"^_tmp1240" = xor i32 %bwnot1239, %load_var_R_EAX_321231
  %"^_tmp1243" = xor i32 %27, %load_var_R_EAX_321231
  %"&_tmp1244" = and i32 %"^_tmp1243", %"^_tmp1240"
  %cast_high1246 = icmp slt i32 %"&_tmp1244", 0
  store i1 %cast_high1246, i1* @R_OF, align 1
  %load_var_R_EAX_321247 = load i32* @R_EAX_32, align 4
  %"^_tmp1249" = xor i32 %load_var_R_EAX_321231, %load_var_R_EDX_321232
  %"^_tmp1251" = xor i32 %"^_tmp1249", %load_var_R_EAX_321247
  %"&_tmp1252" = and i32 %"^_tmp1251", 16
  %"==_tmp1253" = icmp ne i32 %"&_tmp1252", 0
  store i1 %"==_tmp1253", i1* @R_AF, align 1
  %">>_tmp1255" = lshr i32 %load_var_R_EAX_321247, 4
  %load_var_R_EAX_321256 = load i32* @R_EAX_32, align 4
  %"^_tmp1257" = xor i32 %">>_tmp1255", %load_var_R_EAX_321256
  %">>_tmp1258" = lshr i32 %"^_tmp1257", 2
  %"^_tmp1259" = xor i32 %">>_tmp1258", %"^_tmp1257"
  %">>_tmp1260" = lshr i32 %"^_tmp1259", 1
  %"^_tmp1261" = xor i32 %">>_tmp1260", %"^_tmp1259"
  %28 = and i32 %"^_tmp1261", 1
  %bwnot1263 = icmp eq i32 %28, 0
  store i1 %bwnot1263, i1* @R_PF, align 1
  %load_var_R_EAX_321264 = load i32* @R_EAX_32, align 4
  %cast_high1266 = icmp slt i32 %load_var_R_EAX_321264, 0
  store i1 %cast_high1266, i1* @R_SF, align 1
  %"==_tmp1268" = icmp eq i32 %load_var_R_EAX_321264, 0
  store i1 %"==_tmp1268", i1* @R_ZF, align 1
  br label %BB_121

BB_119:                                           ; preds = %BB_118, %switch
  %load_var_R_EBP_321227 = load i32* @R_EBP_32, align 4
  %"+_tmp1228" = add i32 %load_var_R_EBP_321227, -8
  %load_address1229 = inttoptr i32 %"+_tmp1228" to i32*
  %load1230 = load i32* %load_address1229, align 4
  store i32 %load1230, i32* @R_EDX_32, align 4
  br label %BB_120

BB_118:                                           ; preds = %BB_115, %BB_117, %switch, %switch
  %load_var_R_EBP_321223 = load i32* @R_EBP_32, align 4
  %"+_tmp1224" = add i32 %load_var_R_EBP_321223, -4
  %load_address1225 = inttoptr i32 %"+_tmp1224" to i32*
  %load1226 = load i32* %load_address1225, align 4
  store i32 %load1226, i32* @R_EAX_32, align 4
  br label %BB_119

BB_117:                                           ; preds = %BB_113, %switch
  %load_var_R_EBP_321220 = load i32* @R_EBP_32, align 4
  %"+_tmp1221" = add i32 %load_var_R_EBP_321220, -4
  %load_address1222 = inttoptr i32 %"+_tmp1221" to i32*
  store i32 20, i32* %load_address1222, align 4
  br label %BB_118

BB_115:                                           ; preds = %BB_114, %switch.BB_115_crit_edge
  %load_var_R_EAX_321219 = phi i32 [ %load_var_R_EAX_321219.pre, %switch.BB_115_crit_edge ], [ %load1215, %BB_114 ]
  %load_var_R_EBP_321216 = load i32* @R_EBP_32, align 4
  %"+_tmp1217" = add i32 %load_var_R_EBP_321216, -8
  %load_address1218 = inttoptr i32 %"+_tmp1217" to i32*
  store i32 %load_var_R_EAX_321219, i32* %load_address1218, align 4
  br label %BB_118

BB_114:                                           ; preds = %BB_113, %switch
  %load_var_R_EBP_321212 = load i32* @R_EBP_32, align 4
  %"+_tmp1213" = add i32 %load_var_R_EBP_321212, 8
  %load_address1214 = inttoptr i32 %"+_tmp1213" to i32*
  %load1215 = load i32* %load_address1214, align 4
  store i32 %load1215, i32* @R_EAX_32, align 4
  br label %BB_115

BB_113:                                           ; preds = %BB_112, %switch
  %load_var_R_ZF1210 = load i1* @R_ZF, align 1
  br i1 %load_var_R_ZF1210, label %BB_114, label %BB_117

BB_112:                                           ; preds = %BB_111, %switch
  %load_var_R_EBP_321163 = load i32* @R_EBP_32, align 4
  %"+_tmp1164" = add i32 %load_var_R_EBP_321163, 8
  %load_address1165 = inttoptr i32 %"+_tmp1164" to i32*
  %load1166 = load i32* %load_address1165, align 4
  %-_tmp1167 = add i32 %load1166, -10
  %"<_tmp1172" = icmp ult i32 %load1166, 10
  store i1 %"<_tmp1172", i1* @R_CF, align 1
  %load_var_R_EBP_321173 = load i32* @R_EBP_32, align 4
  %"+_tmp1174" = add i32 %load_var_R_EBP_321173, 8
  %load_address1175 = inttoptr i32 %"+_tmp1174" to i32*
  %load1176 = load i32* %load_address1175, align 4
  %29 = sub i32 9, %load1166
  %"&_tmp1183" = and i32 %load1176, %29
  %cast_high1185 = icmp slt i32 %"&_tmp1183", 0
  store i1 %cast_high1185, i1* @R_OF, align 1
  %load_var_R_EBP_321187 = load i32* @R_EBP_32, align 4
  %"+_tmp1188" = add i32 %load_var_R_EBP_321187, 8
  %load_address1189 = inttoptr i32 %"+_tmp1188" to i32*
  %load1190 = load i32* %load_address1189, align 4
  %"^_tmp1191" = xor i32 %load1190, %-_tmp1167
  %"&_tmp1193" = and i32 %"^_tmp1191", 16
  %"==_tmp1194" = icmp ne i32 %"&_tmp1193", 0
  store i1 %"==_tmp1194", i1* @R_AF, align 1
  %">>_tmp1196" = lshr i32 %-_tmp1167, 4
  %"^_tmp1198" = xor i32 %">>_tmp1196", %-_tmp1167
  %">>_tmp1199" = lshr i32 %"^_tmp1198", 2
  %"^_tmp1200" = xor i32 %">>_tmp1199", %"^_tmp1198"
  %">>_tmp1201" = lshr i32 %"^_tmp1200", 1
  %"^_tmp1202" = xor i32 %">>_tmp1201", %"^_tmp1200"
  %30 = and i32 %"^_tmp1202", 1
  %bwnot1204 = icmp eq i32 %30, 0
  store i1 %bwnot1204, i1* @R_PF, align 1
  %cast_high1207 = icmp slt i32 %-_tmp1167, 0
  store i1 %cast_high1207, i1* @R_SF, align 1
  %"==_tmp1209" = icmp eq i32 %-_tmp1167, 0
  store i1 %"==_tmp1209", i1* @R_ZF, align 1
  br label %BB_113

BB_111:                                           ; preds = %BB_110, %switch
  %load_var_R_EBP_321160 = load i32* @R_EBP_32, align 4
  %"+_tmp1161" = add i32 %load_var_R_EBP_321160, -4
  %load_address1162 = inttoptr i32 %"+_tmp1161" to i32*
  store i32 0, i32* %load_address1162, align 4
  br label %BB_112

BB_110:                                           ; preds = %BB_109, %switch
  %load_var_R_EBP_321157 = load i32* @R_EBP_32, align 4
  %"+_tmp1158" = add i32 %load_var_R_EBP_321157, -8
  %load_address1159 = inttoptr i32 %"+_tmp1158" to i32*
  store i32 0, i32* %load_address1159, align 4
  br label %BB_111

BB_109:                                           ; preds = %BB_108, %switch
  %load_var_R_ESP_321124 = load i32* @R_ESP_32, align 4
  %-_tmp1126 = add i32 %load_var_R_ESP_321124, -16
  store i32 %-_tmp1126, i32* @R_ESP_32, align 4
  %"<_tmp1127" = icmp ult i32 %load_var_R_ESP_321124, 16
  store i1 %"<_tmp1127", i1* @R_CF, align 1
  %31 = sub i32 15, %load_var_R_ESP_321124
  %"&_tmp1133" = and i32 %31, %load_var_R_ESP_321124
  %cast_high1135 = icmp slt i32 %"&_tmp1133", 0
  store i1 %cast_high1135, i1* @R_OF, align 1
  %load_var_R_ESP_321136 = load i32* @R_ESP_32, align 4
  %"^_tmp1138" = xor i32 %load_var_R_ESP_321136, %load_var_R_ESP_321124
  %"^_tmp1139" = and i32 %"^_tmp1138", 16
  %"==_tmp1141" = icmp eq i32 %"^_tmp1139", 0
  store i1 %"==_tmp1141", i1* @R_AF, align 1
  %">>_tmp1143" = lshr i32 %load_var_R_ESP_321136, 4
  %"^_tmp1145" = xor i32 %">>_tmp1143", %load_var_R_ESP_321136
  %">>_tmp1146" = lshr i32 %"^_tmp1145", 2
  %"^_tmp1147" = xor i32 %">>_tmp1146", %"^_tmp1145"
  %">>_tmp1148" = lshr i32 %"^_tmp1147", 1
  %"^_tmp1149" = xor i32 %">>_tmp1148", %"^_tmp1147"
  %32 = and i32 %"^_tmp1149", 1
  %bwnot1151 = icmp eq i32 %32, 0
  store i1 %bwnot1151, i1* @R_PF, align 1
  %load_var_R_ESP_321152 = load i32* @R_ESP_32, align 4
  %cast_high1154 = icmp slt i32 %load_var_R_ESP_321152, 0
  store i1 %cast_high1154, i1* @R_SF, align 1
  %"==_tmp1156" = icmp eq i32 %load_var_R_ESP_321152, 0
  store i1 %"==_tmp1156", i1* @R_ZF, align 1
  br label %BB_110

BB_108:                                           ; preds = %BB_107, %switch
  %load_var_R_ESP_321123 = load i32* @R_ESP_32, align 4
  store i32 %load_var_R_ESP_321123, i32* @R_EBP_32, align 4
  br label %BB_109

BB_107:                                           ; preds = %BB_173, %switch
  %load_var_R_EBP_321118 = load i32* @R_EBP_32, align 4
  %load_var_R_ESP_321119 = load i32* @R_ESP_32, align 4
  %-_tmp1120 = add i32 %load_var_R_ESP_321119, -4
  store i32 %-_tmp1120, i32* @R_ESP_32, align 4
  %load_address1122 = inttoptr i32 %-_tmp1120 to i32*
  store i32 %load_var_R_EBP_321118, i32* %load_address1122, align 4
  br label %BB_108

BB_104:                                           ; preds = %switch
  %load_var_R_EBP_321112 = load i32* @R_EBP_32, align 4
  store i32 %load_var_R_EBP_321112, i32* @R_ESP_32, align 4
  %load_address1114 = inttoptr i32 %load_var_R_EBP_321112 to i32*
  %load1115 = load i32* %load_address1114, align 4
  store i32 %load1115, i32* @R_EBP_32, align 4
  %"+_tmp1117" = add i32 %load_var_R_EBP_321112, 4
  store i32 %"+_tmp1117", i32* @R_ESP_32, align 4
  br label %BB_62

BB_103:                                           ; preds = %BB_102, %switch
  %load_var_R_EAX_321106 = load i32* @R_EAX_32, align 4
  %load_var_R_ESP_321107 = load i32* @R_ESP_32, align 4
  %-_tmp1108 = add i32 %load_var_R_ESP_321107, -4
  store i32 %-_tmp1108, i32* @R_ESP_32, align 4
  %load_address1110 = inttoptr i32 %-_tmp1108 to i32*
  store i32 134513873, i32* %load_address1110, align 4
  br label %switch

BB_102:                                           ; preds = %BB_101, %switch
  %load_var_R_ESP_321104 = load i32* @R_ESP_32, align 4
  %load_address1105 = inttoptr i32 %load_var_R_ESP_321104 to i32*
  store i32 134520592, i32* %load_address1105, align 4
  br label %BB_103

BB_101:                                           ; preds = %BB_100, %switch
  %load_var_R_ESP_321071 = load i32* @R_ESP_32, align 4
  %-_tmp1073 = add i32 %load_var_R_ESP_321071, -24
  store i32 %-_tmp1073, i32* @R_ESP_32, align 4
  %"<_tmp1074" = icmp ult i32 %load_var_R_ESP_321071, 24
  store i1 %"<_tmp1074", i1* @R_CF, align 1
  %33 = sub i32 23, %load_var_R_ESP_321071
  %"&_tmp1080" = and i32 %33, %load_var_R_ESP_321071
  %cast_high1082 = icmp slt i32 %"&_tmp1080", 0
  store i1 %cast_high1082, i1* @R_OF, align 1
  %load_var_R_ESP_321083 = load i32* @R_ESP_32, align 4
  %"^_tmp1085" = xor i32 %load_var_R_ESP_321083, %load_var_R_ESP_321071
  %"^_tmp1086" = and i32 %"^_tmp1085", 16
  %"==_tmp1088" = icmp eq i32 %"^_tmp1086", 0
  store i1 %"==_tmp1088", i1* @R_AF, align 1
  %">>_tmp1090" = lshr i32 %load_var_R_ESP_321083, 4
  %"^_tmp1092" = xor i32 %">>_tmp1090", %load_var_R_ESP_321083
  %">>_tmp1093" = lshr i32 %"^_tmp1092", 2
  %"^_tmp1094" = xor i32 %">>_tmp1093", %"^_tmp1092"
  %">>_tmp1095" = lshr i32 %"^_tmp1094", 1
  %"^_tmp1096" = xor i32 %">>_tmp1095", %"^_tmp1094"
  %34 = and i32 %"^_tmp1096", 1
  %bwnot1098 = icmp eq i32 %34, 0
  store i1 %bwnot1098, i1* @R_PF, align 1
  %load_var_R_ESP_321099 = load i32* @R_ESP_32, align 4
  %cast_high1101 = icmp slt i32 %load_var_R_ESP_321099, 0
  store i1 %cast_high1101, i1* @R_SF, align 1
  %"==_tmp1103" = icmp eq i32 %load_var_R_ESP_321099, 0
  store i1 %"==_tmp1103", i1* @R_ZF, align 1
  br label %BB_102

BB_100:                                           ; preds = %BB_99, %switch
  %load_var_R_ESP_321070 = load i32* @R_ESP_32, align 4
  store i32 %load_var_R_ESP_321070, i32* @R_EBP_32, align 4
  br label %BB_101

BB_99:                                            ; preds = %BB_98, %switch
  %load_var_R_EBP_321065 = load i32* @R_EBP_32, align 4
  %load_var_R_ESP_321066 = load i32* @R_ESP_32, align 4
  %-_tmp1067 = add i32 %load_var_R_ESP_321066, -4
  store i32 %-_tmp1067, i32* @R_ESP_32, align 4
  %load_address1069 = inttoptr i32 %-_tmp1067 to i32*
  store i32 %load_var_R_EBP_321065, i32* %load_address1069, align 4
  br label %BB_100

BB_98:                                            ; preds = %BB_97, %switch
  %load_var_R_ZF1064 = load i1* @R_ZF, align 1
  br i1 %load_var_R_ZF1064, label %BB_62, label %BB_99

BB_97:                                            ; preds = %BB_96, %switch.BB_97_crit_edge
  %load_var_R_EAX_321047 = phi i32 [ %load_var_R_EAX_321047.pre, %switch.BB_97_crit_edge ], [ 0, %BB_96 ]
  store i1 false, i1* @R_OF, align 1
  store i1 false, i1* @R_CF, align 1
  %">>_tmp1050" = lshr i32 %load_var_R_EAX_321047, 4
  %"^_tmp1052" = xor i32 %">>_tmp1050", %load_var_R_EAX_321047
  %">>_tmp1053" = lshr i32 %"^_tmp1052", 2
  %"^_tmp1054" = xor i32 %">>_tmp1053", %"^_tmp1052"
  %">>_tmp1055" = lshr i32 %"^_tmp1054", 1
  %"^_tmp1056" = xor i32 %">>_tmp1055", %"^_tmp1054"
  %35 = and i32 %"^_tmp1056", 1
  %bwnot1058 = icmp eq i32 %35, 0
  store i1 %bwnot1058, i1* @R_PF, align 1
  %cast_high1061 = icmp slt i32 %load_var_R_EAX_321047, 0
  store i1 %cast_high1061, i1* @R_SF, align 1
  %"==_tmp1063" = icmp eq i32 %load_var_R_EAX_321047, 0
  store i1 %"==_tmp1063", i1* @R_ZF, align 1
  br label %BB_98

BB_96:                                            ; preds = %BB_95, %switch
  store i32 0, i32* @R_EAX_32, align 4
  br label %BB_97

BB_95:                                            ; preds = %BB_94, %switch
  %load_var_R_ZF1046 = load i1* @R_ZF, align 1
  br i1 %load_var_R_ZF1046, label %BB_62, label %BB_96

BB_94:                                            ; preds = %BB_93, %switch.BB_94_crit_edge
  %load_var_R_EAX_321029 = phi i32 [ %load_var_R_EAX_321029.pre, %switch.BB_94_crit_edge ], [ %load1028, %BB_93 ]
  store i1 false, i1* @R_OF, align 1
  store i1 false, i1* @R_CF, align 1
  %">>_tmp1032" = lshr i32 %load_var_R_EAX_321029, 4
  %"^_tmp1034" = xor i32 %">>_tmp1032", %load_var_R_EAX_321029
  %">>_tmp1035" = lshr i32 %"^_tmp1034", 2
  %"^_tmp1036" = xor i32 %">>_tmp1035", %"^_tmp1034"
  %">>_tmp1037" = lshr i32 %"^_tmp1036", 1
  %"^_tmp1038" = xor i32 %">>_tmp1037", %"^_tmp1036"
  %36 = and i32 %"^_tmp1038", 1
  %bwnot1040 = icmp eq i32 %36, 0
  store i1 %bwnot1040, i1* @R_PF, align 1
  %cast_high1043 = icmp slt i32 %load_var_R_EAX_321029, 0
  store i1 %cast_high1043, i1* @R_SF, align 1
  %"==_tmp1045" = icmp eq i32 %load_var_R_EAX_321029, 0
  store i1 %"==_tmp1045", i1* @R_ZF, align 1
  br label %BB_95

BB_93:                                            ; preds = %switch, %switch
  %load1028 = load i32* inttoptr (i32 134520592 to i32*), align 16
  store i32 %load1028, i32* @R_EAX_32, align 4
  br label %BB_94

BB_92:                                            ; preds = %BB_85, %BB_91, %switch
  %load_var_R_ESP_321022 = load i32* @R_ESP_32, align 4
  %load_address1023 = inttoptr i32 %load_var_R_ESP_321022 to i32*
  %load1024 = load i32* %load_address1023, align 4
  %"+_tmp1026" = add i32 %load_var_R_ESP_321022, 4
  store i32 %"+_tmp1026", i32* @R_ESP_32, align 4
  br label %switch

BB_91:                                            ; preds = %BB_90, %switch
  %load_var_R_EBP_321016 = load i32* @R_EBP_32, align 4
  store i32 %load_var_R_EBP_321016, i32* @R_ESP_32, align 4
  %load_address1018 = inttoptr i32 %load_var_R_EBP_321016 to i32*
  %load1019 = load i32* %load_address1018, align 4
  store i32 %load1019, i32* @R_EBP_32, align 4
  %"+_tmp1021" = add i32 %load_var_R_EBP_321016, 4
  store i32 %"+_tmp1021", i32* @R_ESP_32, align 4
  br label %BB_92

BB_90:                                            ; preds = %switch
  store i8 1, i8* inttoptr (i32 134520872 to i8*), align 8
  br label %BB_91

BB_89:                                            ; preds = %BB_88, %switch
  %load_var_R_ESP_321012 = load i32* @R_ESP_32, align 4
  %-_tmp1013 = add i32 %load_var_R_ESP_321012, -4
  store i32 %-_tmp1013, i32* @R_ESP_32, align 4
  %load_address1015 = inttoptr i32 %-_tmp1013 to i32*
  store i32 134513828, i32* %load_address1015, align 4
  br label %BB_46

BB_88:                                            ; preds = %BB_87, %switch
  %load_var_R_ESP_32979 = load i32* @R_ESP_32, align 4
  %-_tmp981 = add i32 %load_var_R_ESP_32979, -8
  store i32 %-_tmp981, i32* @R_ESP_32, align 4
  %"<_tmp982" = icmp ult i32 %load_var_R_ESP_32979, 8
  store i1 %"<_tmp982", i1* @R_CF, align 1
  %37 = sub i32 7, %load_var_R_ESP_32979
  %"&_tmp988" = and i32 %37, %load_var_R_ESP_32979
  %cast_high990 = icmp slt i32 %"&_tmp988", 0
  store i1 %cast_high990, i1* @R_OF, align 1
  %load_var_R_ESP_32991 = load i32* @R_ESP_32, align 4
  %"^_tmp993" = xor i32 %load_var_R_ESP_32991, %load_var_R_ESP_32979
  %"&_tmp995" = and i32 %"^_tmp993", 16
  %"==_tmp996" = icmp ne i32 %"&_tmp995", 0
  store i1 %"==_tmp996", i1* @R_AF, align 1
  %">>_tmp998" = lshr i32 %load_var_R_ESP_32991, 4
  %"^_tmp1000" = xor i32 %">>_tmp998", %load_var_R_ESP_32991
  %">>_tmp1001" = lshr i32 %"^_tmp1000", 2
  %"^_tmp1002" = xor i32 %">>_tmp1001", %"^_tmp1000"
  %">>_tmp1003" = lshr i32 %"^_tmp1002", 1
  %"^_tmp1004" = xor i32 %">>_tmp1003", %"^_tmp1002"
  %38 = and i32 %"^_tmp1004", 1
  %bwnot1006 = icmp eq i32 %38, 0
  store i1 %bwnot1006, i1* @R_PF, align 1
  %load_var_R_ESP_321007 = load i32* @R_ESP_32, align 4
  %cast_high1009 = icmp slt i32 %load_var_R_ESP_321007, 0
  store i1 %cast_high1009, i1* @R_SF, align 1
  %"==_tmp1011" = icmp eq i32 %load_var_R_ESP_321007, 0
  store i1 %"==_tmp1011", i1* @R_ZF, align 1
  br label %BB_89

BB_87:                                            ; preds = %BB_86, %switch
  %load_var_R_ESP_32978 = load i32* @R_ESP_32, align 4
  store i32 %load_var_R_ESP_32978, i32* @R_EBP_32, align 4
  br label %BB_88

BB_86:                                            ; preds = %BB_85, %switch
  %load_var_R_EBP_32973 = load i32* @R_EBP_32, align 4
  %load_var_R_ESP_32974 = load i32* @R_ESP_32, align 4
  %-_tmp975 = add i32 %load_var_R_ESP_32974, -4
  store i32 %-_tmp975, i32* @R_ESP_32, align 4
  %load_address977 = inttoptr i32 %-_tmp975 to i32*
  store i32 %load_var_R_EBP_32973, i32* %load_address977, align 4
  br label %BB_87

BB_85:                                            ; preds = %BB_84, %switch
  %load_var_R_ZF971 = load i1* @R_ZF, align 1
  br i1 %load_var_R_ZF971, label %BB_86, label %BB_92

BB_84:                                            ; preds = %switch, %switch, %switch
  %load939 = load i8* inttoptr (i32 134520872 to i8*), align 8
  store i1 false, i1* @R_CF, align 1
  %load943 = load i8* inttoptr (i32 134520872 to i8*), align 8
  %39 = xor i8 %load939, -128
  %"&_tmp947" = and i8 %load943, %39
  %cast_high949 = icmp slt i8 %"&_tmp947", 0
  store i1 %cast_high949, i1* @R_OF, align 1
  %load951 = load i8* inttoptr (i32 134520872 to i8*), align 8
  %"^_tmp953" = xor i8 %load951, %load939
  %"&_tmp954" = and i8 %"^_tmp953", 16
  %"==_tmp955" = icmp ne i8 %"&_tmp954", 0
  store i1 %"==_tmp955", i1* @R_AF, align 1
  %">>_tmp957" = lshr i8 %load939, 4
  %"^_tmp959" = xor i8 %">>_tmp957", %load939
  %">>_tmp960" = lshr i8 %"^_tmp959", 2
  %"^_tmp961" = xor i8 %">>_tmp960", %"^_tmp959"
  %">>_tmp962" = lshr i8 %"^_tmp961", 1
  %"^_tmp963" = xor i8 %">>_tmp962", %"^_tmp961"
  %40 = and i8 %"^_tmp963", 1
  %bwnot965 = icmp eq i8 %40, 0
  store i1 %bwnot965, i1* @R_PF, align 1
  %cast_high968 = icmp slt i8 %load939, 0
  store i1 %cast_high968, i1* @R_SF, align 1
  %"==_tmp970" = icmp eq i8 %load939, 0
  store i1 %"==_tmp970", i1* @R_ZF, align 1
  br label %BB_85

BB_81:                                            ; preds = %BB_80, %switch.BB_81_crit_edge
  %load_var_R_ESP_32931 = phi i32 [ %load_var_R_ESP_32931.pre, %switch.BB_81_crit_edge ], [ %"+_tmp930", %BB_80 ]
  %load_address932 = inttoptr i32 %load_var_R_ESP_32931 to i32*
  %load933 = load i32* %load_address932, align 4
  %"+_tmp935" = add i32 %load_var_R_ESP_32931, 4
  store i32 %"+_tmp935", i32* @R_ESP_32, align 4
  br label %switch

BB_80:                                            ; preds = %switch
  %load_var_R_EBP_32925 = load i32* @R_EBP_32, align 4
  store i32 %load_var_R_EBP_32925, i32* @R_ESP_32, align 4
  %load_address927 = inttoptr i32 %load_var_R_EBP_32925 to i32*
  %load928 = load i32* %load_address927, align 4
  store i32 %load928, i32* @R_EBP_32, align 4
  %"+_tmp930" = add i32 %load_var_R_EBP_32925, 4
  store i32 %"+_tmp930", i32* @R_ESP_32, align 4
  br label %BB_81

BB_79:                                            ; preds = %BB_78, %switch
  %load_var_R_EDX_32919 = load i32* @R_EDX_32, align 4
  %load_var_R_ESP_32920 = load i32* @R_ESP_32, align 4
  %-_tmp921 = add i32 %load_var_R_ESP_32920, -4
  store i32 %-_tmp921, i32* @R_ESP_32, align 4
  %load_address923 = inttoptr i32 %-_tmp921 to i32*
  store i32 134513797, i32* %load_address923, align 4
  br label %switch

BB_78:                                            ; preds = %BB_77, %switch
  %load_var_R_ESP_32917 = load i32* @R_ESP_32, align 4
  %load_address918 = inttoptr i32 %load_var_R_ESP_32917 to i32*
  store i32 134520872, i32* %load_address918, align 4
  br label %BB_79

BB_77:                                            ; preds = %BB_76, %switch
  %load_var_R_ESP_32913 = load i32* @R_ESP_32, align 4
  %"+_tmp914" = add i32 %load_var_R_ESP_32913, 4
  %load_address915 = inttoptr i32 %"+_tmp914" to i32*
  %load_var_R_EAX_32916 = load i32* @R_EAX_32, align 4
  store i32 %load_var_R_EAX_32916, i32* %load_address915, align 4
  br label %BB_78

BB_76:                                            ; preds = %BB_75, %switch
  %load_var_R_ESP_32880 = load i32* @R_ESP_32, align 4
  %-_tmp882 = add i32 %load_var_R_ESP_32880, -24
  store i32 %-_tmp882, i32* @R_ESP_32, align 4
  %"<_tmp883" = icmp ult i32 %load_var_R_ESP_32880, 24
  store i1 %"<_tmp883", i1* @R_CF, align 1
  %41 = sub i32 23, %load_var_R_ESP_32880
  %"&_tmp889" = and i32 %41, %load_var_R_ESP_32880
  %cast_high891 = icmp slt i32 %"&_tmp889", 0
  store i1 %cast_high891, i1* @R_OF, align 1
  %load_var_R_ESP_32892 = load i32* @R_ESP_32, align 4
  %"^_tmp894" = xor i32 %load_var_R_ESP_32892, %load_var_R_ESP_32880
  %"^_tmp895" = and i32 %"^_tmp894", 16
  %"==_tmp897" = icmp eq i32 %"^_tmp895", 0
  store i1 %"==_tmp897", i1* @R_AF, align 1
  %">>_tmp899" = lshr i32 %load_var_R_ESP_32892, 4
  %"^_tmp901" = xor i32 %">>_tmp899", %load_var_R_ESP_32892
  %">>_tmp902" = lshr i32 %"^_tmp901", 2
  %"^_tmp903" = xor i32 %">>_tmp902", %"^_tmp901"
  %">>_tmp904" = lshr i32 %"^_tmp903", 1
  %"^_tmp905" = xor i32 %">>_tmp904", %"^_tmp903"
  %42 = and i32 %"^_tmp905", 1
  %bwnot907 = icmp eq i32 %42, 0
  store i1 %bwnot907, i1* @R_PF, align 1
  %load_var_R_ESP_32908 = load i32* @R_ESP_32, align 4
  %cast_high910 = icmp slt i32 %load_var_R_ESP_32908, 0
  store i1 %cast_high910, i1* @R_SF, align 1
  %"==_tmp912" = icmp eq i32 %load_var_R_ESP_32908, 0
  store i1 %"==_tmp912", i1* @R_ZF, align 1
  br label %BB_77

BB_75:                                            ; preds = %BB_74, %switch
  %load_var_R_ESP_32879 = load i32* @R_ESP_32, align 4
  store i32 %load_var_R_ESP_32879, i32* @R_EBP_32, align 4
  br label %BB_76

BB_74:                                            ; preds = %BB_73, %switch
  %load_var_R_EBP_32874 = load i32* @R_EBP_32, align 4
  %load_var_R_ESP_32875 = load i32* @R_ESP_32, align 4
  %-_tmp876 = add i32 %load_var_R_ESP_32875, -4
  store i32 %-_tmp876, i32* @R_ESP_32, align 4
  %load_address878 = inttoptr i32 %-_tmp876 to i32*
  store i32 %load_var_R_EBP_32874, i32* %load_address878, align 4
  br label %BB_75

BB_73:                                            ; preds = %BB_72, %switch
  %load_var_R_ZF873 = load i1* @R_ZF, align 1
  br i1 %load_var_R_ZF873, label %BB_70, label %BB_74

BB_72:                                            ; preds = %BB_71, %switch.BB_72_crit_edge
  %load_var_R_EDX_32856 = phi i32 [ %load_var_R_EDX_32856.pre, %switch.BB_72_crit_edge ], [ 0, %BB_71 ]
  store i1 false, i1* @R_OF, align 1
  store i1 false, i1* @R_CF, align 1
  %">>_tmp859" = lshr i32 %load_var_R_EDX_32856, 4
  %"^_tmp861" = xor i32 %">>_tmp859", %load_var_R_EDX_32856
  %">>_tmp862" = lshr i32 %"^_tmp861", 2
  %"^_tmp863" = xor i32 %">>_tmp862", %"^_tmp861"
  %">>_tmp864" = lshr i32 %"^_tmp863", 1
  %"^_tmp865" = xor i32 %">>_tmp864", %"^_tmp863"
  %43 = and i32 %"^_tmp865", 1
  %bwnot867 = icmp eq i32 %43, 0
  store i1 %bwnot867, i1* @R_PF, align 1
  %cast_high870 = icmp slt i32 %load_var_R_EDX_32856, 0
  store i1 %cast_high870, i1* @R_SF, align 1
  %"==_tmp872" = icmp eq i32 %load_var_R_EDX_32856, 0
  store i1 %"==_tmp872", i1* @R_ZF, align 1
  br label %BB_73

BB_71:                                            ; preds = %BB_69, %switch
  store i32 0, i32* @R_EDX_32, align 4
  br label %BB_72

BB_70:                                            ; preds = %BB_69, %BB_73, %switch
  %load_var_R_ESP_32850 = load i32* @R_ESP_32, align 4
  %load_address851 = inttoptr i32 %load_var_R_ESP_32850 to i32*
  %load852 = load i32* %load_address851, align 4
  %"+_tmp854" = add i32 %load_var_R_ESP_32850, 4
  store i32 %"+_tmp854", i32* @R_ESP_32, align 4
  br label %switch

BB_69:                                            ; preds = %BB_68, %switch
  %load_var_R_ZF848 = load i1* @R_ZF, align 1
  br i1 %load_var_R_ZF848, label %BB_70, label %BB_71

BB_68:                                            ; preds = %BB_67, %switch
  %load_var_R_EAX_32764 = load i32* @R_EAX_32, align 4
  %"$>>_tmp766" = ashr i32 %load_var_R_EAX_32764, 1
  store i32 %"$>>_tmp766", i32* @R_EAX_32, align 4
  %44 = and i32 %load_var_R_EAX_32764, 1
  %cast_high777 = icmp ne i32 %44, 0
  store i1 %cast_high777, i1* @R_CF, align 1
  store i1 false, i1* @R_OF, align 1
  %cast_high806 = icmp slt i32 %"$>>_tmp766", 0
  store i1 %cast_high806, i1* @R_SF, align 1
  %load_var_R_EAX_32816 = load i32* @R_EAX_32, align 4
  %"==_tmp817" = icmp eq i32 %load_var_R_EAX_32816, 0
  store i1 %"==_tmp817", i1* @R_ZF, align 1
  %">>_tmp828" = lshr i32 %load_var_R_EAX_32816, 4
  %"^_tmp830" = xor i32 %">>_tmp828", %load_var_R_EAX_32816
  %">>_tmp831" = lshr i32 %"^_tmp830", 2
  %"^_tmp832" = xor i32 %">>_tmp831", %"^_tmp830"
  %">>_tmp833" = lshr i32 %"^_tmp832", 1
  %"^_tmp834" = xor i32 %">>_tmp833", %"^_tmp832"
  %45 = and i32 %"^_tmp834", 1
  %bwnot836 = icmp eq i32 %45, 0
  store i1 %bwnot836, i1* @R_PF, align 1
  store i1 false, i1* @R_AF, align 1
  br label %BB_69

BB_67:                                            ; preds = %BB_66, %switch
  %load_var_R_EAX_32726 = load i32* @R_EAX_32, align 4
  %load_var_R_EDX_32727 = load i32* @R_EDX_32, align 4
  %uadd2234 = call { i32, i1 } @llvm.uadd.with.overflow.i32(i32 %load_var_R_EAX_32726, i32 %load_var_R_EDX_32727)
  %46 = extractvalue { i32, i1 } %uadd2234, 0
  store i32 %46, i32* @R_EAX_32, align 4
  %"<_tmp731" = extractvalue { i32, i1 } %uadd2234, 1
  store i1 %"<_tmp731", i1* @R_CF, align 1
  %bwnot734 = xor i32 %load_var_R_EAX_32726, -2147483648
  %"^_tmp735" = xor i32 %bwnot734, %load_var_R_EDX_32727
  %"^_tmp738" = xor i32 %46, %load_var_R_EAX_32726
  %"&_tmp739" = and i32 %"^_tmp738", %"^_tmp735"
  %cast_high741 = icmp slt i32 %"&_tmp739", 0
  store i1 %cast_high741, i1* @R_OF, align 1
  %load_var_R_EAX_32742 = load i32* @R_EAX_32, align 4
  %"^_tmp744" = xor i32 %load_var_R_EDX_32727, %load_var_R_EAX_32726
  %"^_tmp746" = xor i32 %"^_tmp744", %load_var_R_EAX_32742
  %"&_tmp747" = and i32 %"^_tmp746", 16
  %"==_tmp748" = icmp ne i32 %"&_tmp747", 0
  store i1 %"==_tmp748", i1* @R_AF, align 1
  %">>_tmp750" = lshr i32 %load_var_R_EAX_32742, 4
  %load_var_R_EAX_32751 = load i32* @R_EAX_32, align 4
  %"^_tmp752" = xor i32 %">>_tmp750", %load_var_R_EAX_32751
  %">>_tmp753" = lshr i32 %"^_tmp752", 2
  %"^_tmp754" = xor i32 %">>_tmp753", %"^_tmp752"
  %">>_tmp755" = lshr i32 %"^_tmp754", 1
  %"^_tmp756" = xor i32 %">>_tmp755", %"^_tmp754"
  %47 = and i32 %"^_tmp756", 1
  %bwnot758 = icmp eq i32 %47, 0
  store i1 %bwnot758, i1* @R_PF, align 1
  %load_var_R_EAX_32759 = load i32* @R_EAX_32, align 4
  %cast_high761 = icmp slt i32 %load_var_R_EAX_32759, 0
  store i1 %cast_high761, i1* @R_SF, align 1
  %"==_tmp763" = icmp eq i32 %load_var_R_EAX_32759, 0
  store i1 %"==_tmp763", i1* @R_ZF, align 1
  br label %BB_68

BB_66:                                            ; preds = %BB_65, %switch.BB_66_crit_edge
  %load_var_R_EDX_32639 = phi i32 [ %load_var_R_EDX_32639.pre, %switch.BB_66_crit_edge ], [ %load_var_R_EAX_32638, %BB_65 ]
  %">>_tmp641" = lshr i32 %load_var_R_EDX_32639, 31
  store i32 %">>_tmp641", i32* @R_EDX_32, align 4
  %48 = and i32 %load_var_R_EDX_32639, 1073741824
  %cast_high652 = icmp ne i32 %48, 0
  store i1 %cast_high652, i1* @R_CF, align 1
  store i1 false, i1* @R_OF, align 1
  store i1 false, i1* @R_SF, align 1
  %"==_tmp695" = icmp eq i32 %">>_tmp641", 0
  store i1 %"==_tmp695", i1* @R_ZF, align 1
  store i1 %"==_tmp695", i1* @R_PF, align 1
  store i1 false, i1* @R_AF, align 1
  br label %BB_67

BB_65:                                            ; preds = %BB_64, %switch
  %load_var_R_EAX_32638 = load i32* @R_EAX_32, align 4
  store i32 %load_var_R_EAX_32638, i32* @R_EDX_32, align 4
  br label %BB_66

BB_64:                                            ; preds = %BB_63, %switch
  %load_var_R_EAX_32560 = load i32* @R_EAX_32, align 4
  %"$>>_tmp" = ashr i32 %load_var_R_EAX_32560, 2
  store i32 %"$>>_tmp", i32* @R_EAX_32, align 4
  %49 = and i32 %load_var_R_EAX_32560, 2
  %cast_high571 = icmp ne i32 %49, 0
  store i1 %cast_high571, i1* @R_CF, align 1
  store i1 false, i1* @R_OF, align 1
  %cast_high598 = icmp slt i32 %"$>>_tmp", 0
  store i1 %cast_high598, i1* @R_SF, align 1
  %"==_tmp609" = icmp eq i32 %"$>>_tmp", 0
  store i1 %"==_tmp609", i1* @R_ZF, align 1
  %">>_tmp619" = lshr i32 %"$>>_tmp", 4
  %"^_tmp621" = xor i32 %">>_tmp619", %"$>>_tmp"
  %">>_tmp622" = lshr i32 %"^_tmp621", 2
  %"^_tmp623" = xor i32 %">>_tmp622", %"^_tmp621"
  %">>_tmp624" = lshr i32 %"^_tmp623", 1
  %"^_tmp625" = xor i32 %">>_tmp624", %"^_tmp623"
  %50 = and i32 %"^_tmp625", 1
  %bwnot627 = icmp eq i32 %50, 0
  store i1 %bwnot627, i1* @R_PF, align 1
  store i1 false, i1* @R_AF, align 1
  br label %BB_65

BB_63:                                            ; preds = %BB_62, %switch.BB_63_crit_edge
  %load_var_R_EAX_32527 = phi i32 [ %load_var_R_EAX_32527.pre, %switch.BB_63_crit_edge ], [ 134520872, %BB_62 ]
  %-_tmp529 = add i32 %load_var_R_EAX_32527, -134520872
  store i32 %-_tmp529, i32* @R_EAX_32, align 4
  %"<_tmp530" = icmp ult i32 %load_var_R_EAX_32527, 134520872
  store i1 %"<_tmp530", i1* @R_CF, align 1
  %51 = sub i32 134520871, %load_var_R_EAX_32527
  %"&_tmp536" = and i32 %51, %load_var_R_EAX_32527
  %cast_high538 = icmp slt i32 %"&_tmp536", 0
  store i1 %cast_high538, i1* @R_OF, align 1
  %load_var_R_EAX_32539 = load i32* @R_EAX_32, align 4
  %"^_tmp541" = xor i32 %load_var_R_EAX_32539, %load_var_R_EAX_32527
  %"&_tmp543" = and i32 %"^_tmp541", 16
  %"==_tmp544" = icmp ne i32 %"&_tmp543", 0
  store i1 %"==_tmp544", i1* @R_AF, align 1
  %">>_tmp546" = lshr i32 %load_var_R_EAX_32539, 4
  %"^_tmp548" = xor i32 %">>_tmp546", %load_var_R_EAX_32539
  %">>_tmp549" = lshr i32 %"^_tmp548", 2
  %"^_tmp550" = xor i32 %">>_tmp549", %"^_tmp548"
  %">>_tmp551" = lshr i32 %"^_tmp550", 1
  %"^_tmp552" = xor i32 %">>_tmp551", %"^_tmp550"
  %52 = and i32 %"^_tmp552", 1
  %bwnot554 = icmp eq i32 %52, 0
  store i1 %bwnot554, i1* @R_PF, align 1
  %load_var_R_EAX_32555 = load i32* @R_EAX_32, align 4
  %cast_high557 = icmp slt i32 %load_var_R_EAX_32555, 0
  store i1 %cast_high557, i1* @R_SF, align 1
  %"==_tmp559" = icmp eq i32 %load_var_R_EAX_32555, 0
  store i1 %"==_tmp559", i1* @R_ZF, align 1
  br label %BB_64

BB_62:                                            ; preds = %BB_95, %BB_98, %BB_104, %switch, %switch, %switch, %switch, %switch
  store i32 134520872, i32* @R_EAX_32, align 4
  br label %BB_63

BB_60:                                            ; preds = %BB_59, %switch.BB_60_crit_edge
  %load_var_R_ESP_32519 = phi i32 [ %load_var_R_ESP_32519.pre, %switch.BB_60_crit_edge ], [ %"+_tmp518", %BB_59 ]
  %load_address520 = inttoptr i32 %load_var_R_ESP_32519 to i32*
  %load521 = load i32* %load_address520, align 4
  %"+_tmp523" = add i32 %load_var_R_ESP_32519, 4
  store i32 %"+_tmp523", i32* @R_ESP_32, align 4
  br label %switch

BB_59:                                            ; preds = %switch
  %load_var_R_EBP_32513 = load i32* @R_EBP_32, align 4
  store i32 %load_var_R_EBP_32513, i32* @R_ESP_32, align 4
  %load_address515 = inttoptr i32 %load_var_R_EBP_32513 to i32*
  %load516 = load i32* %load_address515, align 4
  store i32 %load516, i32* @R_EBP_32, align 4
  %"+_tmp518" = add i32 %load_var_R_EBP_32513, 4
  store i32 %"+_tmp518", i32* @R_ESP_32, align 4
  br label %BB_60

BB_58:                                            ; preds = %BB_57, %switch
  %load_var_R_EAX_32507 = load i32* @R_EAX_32, align 4
  %load_var_R_ESP_32508 = load i32* @R_ESP_32, align 4
  %-_tmp509 = add i32 %load_var_R_ESP_32508, -4
  store i32 %-_tmp509, i32* @R_ESP_32, align 4
  %load_address511 = inttoptr i32 %-_tmp509 to i32*
  store i32 134513736, i32* %load_address511, align 4
  br label %switch

BB_57:                                            ; preds = %BB_56, %switch
  %load_var_R_ESP_32505 = load i32* @R_ESP_32, align 4
  %load_address506 = inttoptr i32 %load_var_R_ESP_32505 to i32*
  store i32 134520872, i32* %load_address506, align 4
  br label %BB_58

BB_56:                                            ; preds = %BB_55, %switch
  %load_var_R_ESP_32472 = load i32* @R_ESP_32, align 4
  %-_tmp474 = add i32 %load_var_R_ESP_32472, -24
  store i32 %-_tmp474, i32* @R_ESP_32, align 4
  %"<_tmp475" = icmp ult i32 %load_var_R_ESP_32472, 24
  store i1 %"<_tmp475", i1* @R_CF, align 1
  %53 = sub i32 23, %load_var_R_ESP_32472
  %"&_tmp481" = and i32 %53, %load_var_R_ESP_32472
  %cast_high483 = icmp slt i32 %"&_tmp481", 0
  store i1 %cast_high483, i1* @R_OF, align 1
  %load_var_R_ESP_32484 = load i32* @R_ESP_32, align 4
  %"^_tmp486" = xor i32 %load_var_R_ESP_32484, %load_var_R_ESP_32472
  %"^_tmp487" = and i32 %"^_tmp486", 16
  %"==_tmp489" = icmp eq i32 %"^_tmp487", 0
  store i1 %"==_tmp489", i1* @R_AF, align 1
  %">>_tmp491" = lshr i32 %load_var_R_ESP_32484, 4
  %"^_tmp493" = xor i32 %">>_tmp491", %load_var_R_ESP_32484
  %">>_tmp494" = lshr i32 %"^_tmp493", 2
  %"^_tmp495" = xor i32 %">>_tmp494", %"^_tmp493"
  %">>_tmp496" = lshr i32 %"^_tmp495", 1
  %"^_tmp497" = xor i32 %">>_tmp496", %"^_tmp495"
  %54 = and i32 %"^_tmp497", 1
  %bwnot499 = icmp eq i32 %54, 0
  store i1 %bwnot499, i1* @R_PF, align 1
  %load_var_R_ESP_32500 = load i32* @R_ESP_32, align 4
  %cast_high502 = icmp slt i32 %load_var_R_ESP_32500, 0
  store i1 %cast_high502, i1* @R_SF, align 1
  %"==_tmp504" = icmp eq i32 %load_var_R_ESP_32500, 0
  store i1 %"==_tmp504", i1* @R_ZF, align 1
  br label %BB_57

BB_55:                                            ; preds = %BB_54, %switch
  %load_var_R_ESP_32471 = load i32* @R_ESP_32, align 4
  store i32 %load_var_R_ESP_32471, i32* @R_EBP_32, align 4
  br label %BB_56

BB_54:                                            ; preds = %BB_53, %switch
  %load_var_R_EBP_32 = load i32* @R_EBP_32, align 4
  %load_var_R_ESP_32467 = load i32* @R_ESP_32, align 4
  %-_tmp468 = add i32 %load_var_R_ESP_32467, -4
  store i32 %-_tmp468, i32* @R_ESP_32, align 4
  %load_address470 = inttoptr i32 %-_tmp468 to i32*
  store i32 %load_var_R_EBP_32, i32* %load_address470, align 4
  br label %BB_55

BB_53:                                            ; preds = %BB_52, %switch
  %load_var_R_ZF466 = load i1* @R_ZF, align 1
  br i1 %load_var_R_ZF466, label %BB_50, label %BB_54

BB_52:                                            ; preds = %BB_51, %switch.BB_52_crit_edge
  %load_var_R_EAX_32449 = phi i32 [ %load_var_R_EAX_32449.pre, %switch.BB_52_crit_edge ], [ 0, %BB_51 ]
  store i1 false, i1* @R_OF, align 1
  store i1 false, i1* @R_CF, align 1
  %">>_tmp452" = lshr i32 %load_var_R_EAX_32449, 4
  %"^_tmp454" = xor i32 %">>_tmp452", %load_var_R_EAX_32449
  %">>_tmp455" = lshr i32 %"^_tmp454", 2
  %"^_tmp456" = xor i32 %">>_tmp455", %"^_tmp454"
  %">>_tmp457" = lshr i32 %"^_tmp456", 1
  %"^_tmp458" = xor i32 %">>_tmp457", %"^_tmp456"
  %55 = and i32 %"^_tmp458", 1
  %bwnot460 = icmp eq i32 %55, 0
  store i1 %bwnot460, i1* @R_PF, align 1
  %cast_high463 = icmp slt i32 %load_var_R_EAX_32449, 0
  store i1 %cast_high463, i1* @R_SF, align 1
  %"==_tmp465" = icmp eq i32 %load_var_R_EAX_32449, 0
  store i1 %"==_tmp465", i1* @R_ZF, align 1
  br label %BB_53

BB_51:                                            ; preds = %BB_49, %switch
  store i32 0, i32* @R_EAX_32, align 4
  br label %BB_52

BB_50:                                            ; preds = %BB_49, %BB_53, %switch
  %load_var_R_ESP_32443 = load i32* @R_ESP_32, align 4
  %load_address444 = inttoptr i32 %load_var_R_ESP_32443 to i32*
  %load445 = load i32* %load_address444, align 4
  %"+_tmp447" = add i32 %load_var_R_ESP_32443, 4
  store i32 %"+_tmp447", i32* @R_ESP_32, align 4
  br label %switch

BB_49:                                            ; preds = %BB_48, %switch
  %load_var_R_CF = load i1* @R_CF, align 1
  %load_var_R_ZF441 = load i1* @R_ZF, align 1
  %"|_tmp" = or i1 %load_var_R_CF, %load_var_R_ZF441
  br i1 %"|_tmp", label %BB_50, label %BB_51

BB_48:                                            ; preds = %BB_47, %switch
  %load_var_R_EAX_32409 = load i32* @R_EAX_32, align 4
  %-_tmp410 = add i32 %load_var_R_EAX_32409, -6
  %"<_tmp412" = icmp ult i32 %load_var_R_EAX_32409, 6
  store i1 %"<_tmp412", i1* @R_CF, align 1
  %56 = sub i32 5, %load_var_R_EAX_32409
  %"&_tmp417" = and i32 %56, %load_var_R_EAX_32409
  %cast_high419 = icmp slt i32 %"&_tmp417", 0
  store i1 %cast_high419, i1* @R_OF, align 1
  %load_var_R_EAX_32421 = load i32* @R_EAX_32, align 4
  %"^_tmp422" = xor i32 %-_tmp410, %load_var_R_EAX_32421
  %"&_tmp424" = and i32 %"^_tmp422", 16
  %"==_tmp425" = icmp ne i32 %"&_tmp424", 0
  store i1 %"==_tmp425", i1* @R_AF, align 1
  %">>_tmp427" = lshr i32 %-_tmp410, 4
  %"^_tmp429" = xor i32 %">>_tmp427", %-_tmp410
  %">>_tmp430" = lshr i32 %"^_tmp429", 2
  %"^_tmp431" = xor i32 %">>_tmp430", %"^_tmp429"
  %">>_tmp432" = lshr i32 %"^_tmp431", 1
  %"^_tmp433" = xor i32 %">>_tmp432", %"^_tmp431"
  %57 = and i32 %"^_tmp433", 1
  %bwnot435 = icmp eq i32 %57, 0
  store i1 %bwnot435, i1* @R_PF, align 1
  %cast_high438 = icmp slt i32 %-_tmp410, 0
  store i1 %cast_high438, i1* @R_SF, align 1
  %"==_tmp440" = icmp eq i32 %-_tmp410, 0
  store i1 %"==_tmp440", i1* @R_ZF, align 1
  br label %BB_49

BB_47:                                            ; preds = %BB_46, %switch.BB_47_crit_edge
  %load_var_R_EAX_32376 = phi i32 [ %load_var_R_EAX_32376.pre, %switch.BB_47_crit_edge ], [ 134520875, %BB_46 ]
  %-_tmp378 = add i32 %load_var_R_EAX_32376, -134520872
  store i32 %-_tmp378, i32* @R_EAX_32, align 4
  %"<_tmp379" = icmp ult i32 %load_var_R_EAX_32376, 134520872
  store i1 %"<_tmp379", i1* @R_CF, align 1
  %58 = sub i32 134520871, %load_var_R_EAX_32376
  %"&_tmp385" = and i32 %58, %load_var_R_EAX_32376
  %cast_high387 = icmp slt i32 %"&_tmp385", 0
  store i1 %cast_high387, i1* @R_OF, align 1
  %load_var_R_EAX_32388 = load i32* @R_EAX_32, align 4
  %"^_tmp390" = xor i32 %load_var_R_EAX_32388, %load_var_R_EAX_32376
  %"&_tmp392" = and i32 %"^_tmp390", 16
  %"==_tmp393" = icmp ne i32 %"&_tmp392", 0
  store i1 %"==_tmp393", i1* @R_AF, align 1
  %">>_tmp395" = lshr i32 %load_var_R_EAX_32388, 4
  %"^_tmp397" = xor i32 %">>_tmp395", %load_var_R_EAX_32388
  %">>_tmp398" = lshr i32 %"^_tmp397", 2
  %"^_tmp399" = xor i32 %">>_tmp398", %"^_tmp397"
  %">>_tmp400" = lshr i32 %"^_tmp399", 1
  %"^_tmp401" = xor i32 %">>_tmp400", %"^_tmp399"
  %59 = and i32 %"^_tmp401", 1
  %bwnot403 = icmp eq i32 %59, 0
  store i1 %bwnot403, i1* @R_PF, align 1
  %load_var_R_EAX_32404 = load i32* @R_EAX_32, align 4
  %cast_high406 = icmp slt i32 %load_var_R_EAX_32404, 0
  store i1 %cast_high406, i1* @R_SF, align 1
  %"==_tmp408" = icmp eq i32 %load_var_R_EAX_32404, 0
  store i1 %"==_tmp408", i1* @R_ZF, align 1
  br label %BB_48

BB_46:                                            ; preds = %BB_89, %switch, %switch, %switch, %switch, %switch, %switch, %switch
  store i32 134520875, i32* @R_EAX_32, align 4
  br label %BB_47

BB_45:                                            ; preds = %BB_44, %switch
  %load_var_R_ESP_32370 = load i32* @R_ESP_32, align 4
  %load_address371 = inttoptr i32 %load_var_R_ESP_32370 to i32*
  %load372 = load i32* %load_address371, align 4
  %"+_tmp374" = add i32 %load_var_R_ESP_32370, 4
  store i32 %"+_tmp374", i32* @R_ESP_32, align 4
  br label %switch

BB_44:                                            ; preds = %BB_2, %BB_204, %BB_235, %switch, %switch, %switch, %switch, %switch, %switch, %switch, %switch
  %load_var_R_ESP_32367 = load i32* @R_ESP_32, align 4
  %load_address368 = inttoptr i32 %load_var_R_ESP_32367 to i32*
  %load369 = load i32* %load_address368, align 4
  store i32 %load369, i32* @R_EBX_32, align 4
  br label %BB_45

BB_43:                                            ; preds = %switch
  %load_var_R_EAX_322228 = load i32* @R_EAX_32, align 4
  ret i32 %load_var_R_EAX_322228

BB_42:                                            ; preds = %BB_41, %switch
  %load_var_R_ESP_32363 = load i32* @R_ESP_32, align 4
  %-_tmp364 = add i32 %load_var_R_ESP_32363, -4
  store i32 %-_tmp364, i32* @R_ESP_32, align 4
  %load_address366 = inttoptr i32 %-_tmp364 to i32*
  store i32 134513665, i32* %load_address366, align 4
  br label %BB_21

BB_41:                                            ; preds = %BB_40, %switch
  %load_var_R_ESP_32359 = load i32* @R_ESP_32, align 4
  %-_tmp360 = add i32 %load_var_R_ESP_32359, -4
  store i32 %-_tmp360, i32* @R_ESP_32, align 4
  %load_address362 = inttoptr i32 %-_tmp360 to i32*
  store i32 134514006, i32* %load_address362, align 4
  br label %BB_42

BB_40:                                            ; preds = %BB_39, %switch
  %load_var_R_ESI_32 = load i32* @R_ESI_32, align 4
  %load_var_R_ESP_32355 = load i32* @R_ESP_32, align 4
  %-_tmp356 = add i32 %load_var_R_ESP_32355, -4
  store i32 %-_tmp356, i32* @R_ESP_32, align 4
  %load_address358 = inttoptr i32 %-_tmp356 to i32*
  store i32 %load_var_R_ESI_32, i32* %load_address358, align 4
  br label %BB_41

BB_39:                                            ; preds = %BB_38, %switch
  %load_var_R_ECX_32 = load i32* @R_ECX_32, align 4
  %load_var_R_ESP_32351 = load i32* @R_ESP_32, align 4
  %-_tmp352 = add i32 %load_var_R_ESP_32351, -4
  store i32 %-_tmp352, i32* @R_ESP_32, align 4
  %load_address354 = inttoptr i32 %-_tmp352 to i32*
  store i32 %load_var_R_ECX_32, i32* %load_address354, align 4
  br label %BB_40

BB_38:                                            ; preds = %BB_37, %switch
  %load_var_R_ESP_32347 = load i32* @R_ESP_32, align 4
  %-_tmp348 = add i32 %load_var_R_ESP_32347, -4
  store i32 %-_tmp348, i32* @R_ESP_32, align 4
  %load_address350 = inttoptr i32 %-_tmp348 to i32*
  store i32 134514240, i32* %load_address350, align 4
  br label %BB_39

BB_37:                                            ; preds = %BB_36, %switch
  %load_var_R_ESP_32343 = load i32* @R_ESP_32, align 4
  %-_tmp344 = add i32 %load_var_R_ESP_32343, -4
  store i32 %-_tmp344, i32* @R_ESP_32, align 4
  %load_address346 = inttoptr i32 %-_tmp344 to i32*
  store i32 134514352, i32* %load_address346, align 4
  br label %BB_38

BB_36:                                            ; preds = %BB_35, %switch
  %load_var_R_EDX_32 = load i32* @R_EDX_32, align 4
  %load_var_R_ESP_32339 = load i32* @R_ESP_32, align 4
  %-_tmp340 = add i32 %load_var_R_ESP_32339, -4
  store i32 %-_tmp340, i32* @R_ESP_32, align 4
  %load_address342 = inttoptr i32 %-_tmp340 to i32*
  store i32 %load_var_R_EDX_32, i32* %load_address342, align 4
  br label %BB_37

BB_35:                                            ; preds = %BB_34, %switch
  %load_var_R_ESP_32334 = load i32* @R_ESP_32, align 4
  %-_tmp336 = add i32 %load_var_R_ESP_32334, -4
  store i32 %-_tmp336, i32* @R_ESP_32, align 4
  %load_address338 = inttoptr i32 %-_tmp336 to i32*
  store i32 %load_var_R_ESP_32334, i32* %load_address338, align 4
  br label %BB_36

BB_34:                                            ; preds = %BB_33, %switch
  %load_var_R_EAX_32329 = load i32* @R_EAX_32, align 4
  %load_var_R_ESP_32330 = load i32* @R_ESP_32, align 4
  %-_tmp331 = add i32 %load_var_R_ESP_32330, -4
  store i32 %-_tmp331, i32* @R_ESP_32, align 4
  %load_address333 = inttoptr i32 %-_tmp331 to i32*
  store i32 %load_var_R_EAX_32329, i32* %load_address333, align 4
  br label %BB_35

BB_33:                                            ; preds = %BB_32, %switch
  %load_var_R_ESP_32312 = load i32* @R_ESP_32, align 4
  %"&_tmp313" = and i32 %load_var_R_ESP_32312, -16
  store i32 %"&_tmp313", i32* @R_ESP_32, align 4
  store i1 false, i1* @R_OF, align 1
  store i1 false, i1* @R_CF, align 1
  %">>_tmp315" = lshr i32 %load_var_R_ESP_32312, 4
  %"^_tmp317" = xor i32 %">>_tmp315", %"&_tmp313"
  %">>_tmp318" = lshr i32 %"^_tmp317", 2
  %"^_tmp319" = xor i32 %">>_tmp318", %"^_tmp317"
  %">>_tmp320" = lshr i32 %"^_tmp319", 1
  %"^_tmp321" = xor i32 %">>_tmp320", %"^_tmp319"
  %60 = and i32 %"^_tmp321", 1
  %bwnot323 = icmp eq i32 %60, 0
  store i1 %bwnot323, i1* @R_PF, align 1
  %load_var_R_ESP_32324 = load i32* @R_ESP_32, align 4
  %cast_high326 = icmp slt i32 %load_var_R_ESP_32324, 0
  store i1 %cast_high326, i1* @R_SF, align 1
  %"==_tmp328" = icmp eq i32 %load_var_R_ESP_32324, 0
  store i1 %"==_tmp328", i1* @R_ZF, align 1
  br label %BB_34

BB_32:                                            ; preds = %BB_31, %switch.BB_32_crit_edge
  %load_var_R_ESP_32311 = phi i32 [ %load_var_R_ESP_32311.pre, %switch.BB_32_crit_edge ], [ %"+_tmp310", %BB_31 ]
  store i32 %load_var_R_ESP_32311, i32* @R_ECX_32, align 4
  br label %BB_33

BB_31:                                            ; preds = %BB_30, %switch
  %load_var_R_ESP_32306 = load i32* @R_ESP_32, align 4
  %load_address307 = inttoptr i32 %load_var_R_ESP_32306 to i32*
  %load308 = load i32* %load_address307, align 4
  store i32 %load308, i32* @R_ESI_32, align 4
  %"+_tmp310" = add i32 %load_var_R_ESP_32306, 4
  store i32 %"+_tmp310", i32* @R_ESP_32, align 4
  br label %BB_32

BB_30:                                            ; preds = %switch
  store i32 0, i32* @R_EBP_32, align 4
  store i1 true, i1* @R_ZF, align 1
  store i1 true, i1* @R_PF, align 1
  store i1 false, i1* @R_OF, align 1
  store i1 false, i1* @R_CF, align 1
  store i1 false, i1* @R_SF, align 1
  br label %BB_31

BB_28:                                            ; preds = %switch
  %load_var_R_ESP_32302 = load i32* @R_ESP_32, align 4
  %-_tmp303 = add i32 %load_var_R_ESP_32302, -4
  store i32 %-_tmp303, i32* @R_ESP_32, align 4
  %load_address305 = inttoptr i32 %-_tmp303 to i32*
  store i32 32, i32* %load_address305, align 4
  br label %BB_11

BB_27:                                            ; preds = %BB_166, %BB_170, %switch
  call void @"*804a01c__isoc99_scanf@GLIBC_2.7"(i32 0)
  %load300 = load i32* inttoptr (i32 134520860 to i32*), align 4
  br label %switch

BB_25:                                            ; preds = %switch
  %load_var_R_ESP_32293 = load i32* @R_ESP_32, align 4
  %-_tmp294 = add i32 %load_var_R_ESP_32293, -4
  store i32 %-_tmp294, i32* @R_ESP_32, align 4
  %load_address296 = inttoptr i32 %-_tmp294 to i32*
  store i32 24, i32* %load_address296, align 4
  br label %BB_11

BB_24:                                            ; preds = %BB_187, %BB_191, %switch
  call void @"*804a018fopen@GLIBC_2.1"(i32 0)
  %load291 = load i32* inttoptr (i32 134520856 to i32*), align 8
  br label %switch

BB_22:                                            ; preds = %switch
  %load_var_R_ESP_32284 = load i32* @R_ESP_32, align 4
  %-_tmp285 = add i32 %load_var_R_ESP_32284, -4
  store i32 %-_tmp285, i32* @R_ESP_32, align 4
  %load_address287 = inttoptr i32 %-_tmp285 to i32*
  store i32 16, i32* %load_address287, align 4
  br label %BB_11

BB_21:                                            ; preds = %BB_42, %switch
  call void @"*804a014__libc_start_main@GLIBC_2.0"(i32 0)
  %load282 = load i32* inttoptr (i32 134520852 to i32*), align 4
  br label %switch

BB_19:                                            ; preds = %switch
  %load_var_R_ESP_32275 = load i32* @R_ESP_32, align 4
  %-_tmp276 = add i32 %load_var_R_ESP_32275, -4
  store i32 %-_tmp276, i32* @R_ESP_32, align 4
  %load_address278 = inttoptr i32 %-_tmp276 to i32*
  store i32 8, i32* %load_address278, align 4
  br label %BB_11

BB_18:                                            ; preds = %BB_7, %switch
  call void @"*804a010__gmon_start__"(i32 0)
  %load273 = load i32* inttoptr (i32 134520848 to i32*), align 16
  br label %switch

BB_16:                                            ; preds = %switch
  %load_var_R_ESP_32266 = load i32* @R_ESP_32, align 4
  %-_tmp267 = add i32 %load_var_R_ESP_32266, -4
  store i32 %-_tmp267, i32* @R_ESP_32, align 4
  %load_address269 = inttoptr i32 %-_tmp267 to i32*
  store i32 0, i32* %load_address269, align 4
  br label %BB_11

BB_15:                                            ; preds = %BB_14, %BB_196, %switch
  call void @"*804a00c__gmon_start__"(i32 0)
  %load264 = load i32* inttoptr (i32 134520844 to i32*), align 4
  br label %switch

BB_14:                                            ; preds = %BB_13, %switch
  %load_var_R_EAX_32202 = load i32* @R_EAX_32, align 4
  %load_address203 = inttoptr i32 %load_var_R_EAX_32202 to i8*
  %load204 = load i8* %load_address203, align 1
  %cast_low206 = trunc i32 %load_var_R_EAX_32202 to i8
  %"+_tmp212" = add i8 %load204, %cast_low206
  store i8 %"+_tmp212", i8* %load_address203, align 1
  %load_var_R_EAX_32213 = load i32* @R_EAX_32, align 4
  %load_address214 = inttoptr i32 %load_var_R_EAX_32213 to i8*
  %load215 = load i8* %load_address214, align 1
  %"<_tmp216" = icmp ult i8 %load215, %load204
  store i1 %"<_tmp216", i1* @R_CF, align 1
  %bwnot219 = xor i8 %load204, -128
  %"^_tmp220" = xor i8 %bwnot219, %cast_low206
  %load_var_R_EAX_32222 = load i32* @R_EAX_32, align 4
  %load_address223 = inttoptr i32 %load_var_R_EAX_32222 to i8*
  %load224 = load i8* %load_address223, align 1
  %"^_tmp225" = xor i8 %load224, %load204
  %"&_tmp226" = and i8 %"^_tmp225", %"^_tmp220"
  %cast_high228 = icmp slt i8 %"&_tmp226", 0
  store i1 %cast_high228, i1* @R_OF, align 1
  %load_var_R_EAX_32229 = load i32* @R_EAX_32, align 4
  %load_address230 = inttoptr i32 %load_var_R_EAX_32229 to i8*
  %load231 = load i8* %load_address230, align 1
  %"^_tmp233" = xor i8 %load204, %cast_low206
  %"^_tmp235" = xor i8 %"^_tmp233", %load231
  %"&_tmp236" = and i8 %"^_tmp235", 16
  %"==_tmp237" = icmp ne i8 %"&_tmp236", 0
  store i1 %"==_tmp237", i1* @R_AF, align 1
  %load_var_R_EAX_32238 = load i32* @R_EAX_32, align 4
  %load_address239 = inttoptr i32 %load_var_R_EAX_32238 to i8*
  %load240 = load i8* %load_address239, align 1
  %">>_tmp241" = lshr i8 %load240, 4
  %"^_tmp245" = xor i8 %">>_tmp241", %load240
  %">>_tmp246" = lshr i8 %"^_tmp245", 2
  %"^_tmp247" = xor i8 %">>_tmp246", %"^_tmp245"
  %">>_tmp248" = lshr i8 %"^_tmp247", 1
  %"^_tmp249" = xor i8 %">>_tmp248", %"^_tmp247"
  %61 = and i8 %"^_tmp249", 1
  %bwnot251 = icmp eq i8 %61, 0
  store i1 %bwnot251, i1* @R_PF, align 1
  %load_var_R_EAX_32252 = load i32* @R_EAX_32, align 4
  %load_address253 = inttoptr i32 %load_var_R_EAX_32252 to i8*
  %load254 = load i8* %load_address253, align 1
  %cast_high256 = icmp slt i8 %load254, 0
  store i1 %cast_high256, i1* @R_SF, align 1
  %load259 = load i8* %load_address253, align 1
  %"==_tmp260" = icmp eq i8 %load259, 0
  store i1 %"==_tmp260", i1* @R_ZF, align 1
  br label %BB_15

BB_13:                                            ; preds = %switch
  %load_var_R_EAX_32143 = load i32* @R_EAX_32, align 4
  %load_address144 = inttoptr i32 %load_var_R_EAX_32143 to i8*
  %load145 = load i8* %load_address144, align 1
  %cast_low147 = trunc i32 %load_var_R_EAX_32143 to i8
  %"+_tmp153" = add i8 %load145, %cast_low147
  store i8 %"+_tmp153", i8* %load_address144, align 1
  %load_var_R_EAX_32154 = load i32* @R_EAX_32, align 4
  %load_address155 = inttoptr i32 %load_var_R_EAX_32154 to i8*
  %load156 = load i8* %load_address155, align 1
  %"<_tmp157" = icmp ult i8 %load156, %load145
  store i1 %"<_tmp157", i1* @R_CF, align 1
  %bwnot160 = xor i8 %load145, -128
  %"^_tmp161" = xor i8 %bwnot160, %cast_low147
  %load_var_R_EAX_32163 = load i32* @R_EAX_32, align 4
  %load_address164 = inttoptr i32 %load_var_R_EAX_32163 to i8*
  %load165 = load i8* %load_address164, align 1
  %"^_tmp166" = xor i8 %load165, %load145
  %"&_tmp167" = and i8 %"^_tmp166", %"^_tmp161"
  %cast_high169 = icmp slt i8 %"&_tmp167", 0
  store i1 %cast_high169, i1* @R_OF, align 1
  %load_var_R_EAX_32170 = load i32* @R_EAX_32, align 4
  %load_address171 = inttoptr i32 %load_var_R_EAX_32170 to i8*
  %load172 = load i8* %load_address171, align 1
  %"^_tmp174" = xor i8 %load145, %cast_low147
  %"^_tmp176" = xor i8 %"^_tmp174", %load172
  %"&_tmp177" = and i8 %"^_tmp176", 16
  %"==_tmp178" = icmp ne i8 %"&_tmp177", 0
  store i1 %"==_tmp178", i1* @R_AF, align 1
  %load_var_R_EAX_32179 = load i32* @R_EAX_32, align 4
  %load_address180 = inttoptr i32 %load_var_R_EAX_32179 to i8*
  %load181 = load i8* %load_address180, align 1
  %">>_tmp182" = lshr i8 %load181, 4
  %"^_tmp186" = xor i8 %">>_tmp182", %load181
  %">>_tmp187" = lshr i8 %"^_tmp186", 2
  %"^_tmp188" = xor i8 %">>_tmp187", %"^_tmp186"
  %">>_tmp189" = lshr i8 %"^_tmp188", 1
  %"^_tmp190" = xor i8 %">>_tmp189", %"^_tmp188"
  %62 = and i8 %"^_tmp190", 1
  %bwnot192 = icmp eq i8 %62, 0
  store i1 %bwnot192, i1* @R_PF, align 1
  %load_var_R_EAX_32193 = load i32* @R_EAX_32, align 4
  %load_address194 = inttoptr i32 %load_var_R_EAX_32193 to i8*
  %load195 = load i8* %load_address194, align 1
  %cast_high197 = icmp slt i8 %load195, 0
  store i1 %cast_high197, i1* @R_SF, align 1
  %load200 = load i8* %load_address194, align 1
  %"==_tmp201" = icmp eq i8 %load200, 0
  store i1 %"==_tmp201", i1* @R_ZF, align 1
  br label %BB_14

BB_12:                                            ; preds = %BB_11, %switch
  %load141 = load i32* inttoptr (i32 134520840 to i32*), align 8
  br label %switch

BB_11:                                            ; preds = %BB_16, %BB_19, %BB_22, %BB_25, %BB_28, %switch, %switch, %switch, %switch, %switch, %switch
  %load136 = load i32* inttoptr (i32 134520836 to i32*), align 4
  %load_var_R_ESP_32137 = load i32* @R_ESP_32, align 4
  %-_tmp138 = add i32 %load_var_R_ESP_32137, -4
  store i32 %-_tmp138, i32* @R_ESP_32, align 4
  %load_address140 = inttoptr i32 %-_tmp138 to i32*
  store i32 %load136, i32* %load_address140, align 4
  br label %BB_12

BB_10:                                            ; preds = %BB_9, %switch.BB_10_crit_edge
  %load_var_R_ESP_32131 = phi i32 [ %load_var_R_ESP_32131.pre, %switch.BB_10_crit_edge ], [ %"+_tmp130", %BB_9 ]
  %load_address132 = inttoptr i32 %load_var_R_ESP_32131 to i32*
  %load133 = load i32* %load_address132, align 4
  %"+_tmp135" = add i32 %load_var_R_ESP_32131, 4
  store i32 %"+_tmp135", i32* @R_ESP_32, align 4
  br label %switch

BB_9:                                             ; preds = %BB_8, %switch
  %load_var_R_ESP_32126 = load i32* @R_ESP_32, align 4
  %load_address127 = inttoptr i32 %load_var_R_ESP_32126 to i32*
  %load128 = load i32* %load_address127, align 4
  store i32 %load128, i32* @R_EBX_32, align 4
  %"+_tmp130" = add i32 %load_var_R_ESP_32126, 4
  store i32 %"+_tmp130", i32* @R_ESP_32, align 4
  br label %BB_10

BB_8:                                             ; preds = %BB_6, %switch
  %load_var_R_ESP_3289 = load i32* @R_ESP_32, align 4
  %uadd2235 = call { i32, i1 } @llvm.uadd.with.overflow.i32(i32 %load_var_R_ESP_3289, i32 8)
  %63 = extractvalue { i32, i1 } %uadd2235, 0
  store i32 %63, i32* @R_ESP_32, align 4
  %"<_tmp93" = extractvalue { i32, i1 } %uadd2235, 1
  store i1 %"<_tmp93", i1* @R_CF, align 1
  %"^_tmp97" = xor i32 %load_var_R_ESP_3289, -2147483648
  %"^_tmp100" = xor i32 %63, %load_var_R_ESP_3289
  %"&_tmp101" = and i32 %"^_tmp100", %"^_tmp97"
  %cast_high103 = icmp slt i32 %"&_tmp101", 0
  store i1 %cast_high103, i1* @R_OF, align 1
  %load_var_R_ESP_32104 = load i32* @R_ESP_32, align 4
  %"^_tmp106" = xor i32 %load_var_R_ESP_32104, %load_var_R_ESP_3289
  %"&_tmp109" = and i32 %"^_tmp106", 16
  %"==_tmp110" = icmp ne i32 %"&_tmp109", 0
  store i1 %"==_tmp110", i1* @R_AF, align 1
  %">>_tmp112" = lshr i32 %load_var_R_ESP_32104, 4
  %"^_tmp114" = xor i32 %">>_tmp112", %load_var_R_ESP_32104
  %">>_tmp115" = lshr i32 %"^_tmp114", 2
  %"^_tmp116" = xor i32 %">>_tmp115", %"^_tmp114"
  %">>_tmp117" = lshr i32 %"^_tmp116", 1
  %"^_tmp118" = xor i32 %">>_tmp117", %"^_tmp116"
  %64 = and i32 %"^_tmp118", 1
  %bwnot120 = icmp eq i32 %64, 0
  store i1 %bwnot120, i1* @R_PF, align 1
  %load_var_R_ESP_32121 = load i32* @R_ESP_32, align 4
  %cast_high123 = icmp slt i32 %load_var_R_ESP_32121, 0
  store i1 %cast_high123, i1* @R_SF, align 1
  %"==_tmp125" = icmp eq i32 %load_var_R_ESP_32121, 0
  store i1 %"==_tmp125", i1* @R_ZF, align 1
  br label %BB_9

BB_7:                                             ; preds = %BB_6, %switch
  %load_var_R_ESP_3285 = load i32* @R_ESP_32, align 4
  %-_tmp86 = add i32 %load_var_R_ESP_3285, -4
  store i32 %-_tmp86, i32* @R_ESP_32, align 4
  %load_address88 = inttoptr i32 %-_tmp86 to i32*
  store i32 134513522, i32* %load_address88, align 4
  br label %BB_18

BB_6:                                             ; preds = %BB_5, %switch
  %load_var_R_ZF = load i1* @R_ZF, align 1
  br i1 %load_var_R_ZF, label %BB_8, label %BB_7

BB_5:                                             ; preds = %BB_4, %switch.BB_5_crit_edge
  %load_var_R_EAX_32 = phi i32 [ %load_var_R_EAX_32.pre, %switch.BB_5_crit_edge ], [ %load, %BB_4 ]
  store i1 false, i1* @R_OF, align 1
  store i1 false, i1* @R_CF, align 1
  %">>_tmp71" = lshr i32 %load_var_R_EAX_32, 4
  %"^_tmp73" = xor i32 %">>_tmp71", %load_var_R_EAX_32
  %">>_tmp74" = lshr i32 %"^_tmp73", 2
  %"^_tmp75" = xor i32 %">>_tmp74", %"^_tmp73"
  %">>_tmp76" = lshr i32 %"^_tmp75", 1
  %"^_tmp77" = xor i32 %">>_tmp76", %"^_tmp75"
  %65 = and i32 %"^_tmp77", 1
  %bwnot79 = icmp eq i32 %65, 0
  store i1 %bwnot79, i1* @R_PF, align 1
  %cast_high82 = icmp slt i32 %load_var_R_EAX_32, 0
  store i1 %cast_high82, i1* @R_SF, align 1
  %"==_tmp84" = icmp eq i32 %load_var_R_EAX_32, 0
  store i1 %"==_tmp84", i1* @R_ZF, align 1
  br label %BB_6

BB_4:                                             ; preds = %BB_3, %switch
  %load_var_R_EBX_3266 = load i32* @R_EBX_32, align 4
  %"+_tmp67" = add i32 %load_var_R_EBX_3266, -4
  %load_address68 = inttoptr i32 %"+_tmp67" to i32*
  %load = load i32* %load_address68, align 4
  store i32 %load, i32* @R_EAX_32, align 4
  br label %BB_5

BB_3:                                             ; preds = %switch
  %load_var_R_EBX_3230 = load i32* @R_EBX_32, align 4
  %uadd2236 = call { i32, i1 } @llvm.uadd.with.overflow.i32(i32 %load_var_R_EBX_3230, i32 7331)
  %66 = extractvalue { i32, i1 } %uadd2236, 0
  store i32 %66, i32* @R_EBX_32, align 4
  %"<_tmp33" = extractvalue { i32, i1 } %uadd2236, 1
  store i1 %"<_tmp33", i1* @R_CF, align 1
  %"^_tmp37" = xor i32 %load_var_R_EBX_3230, -2147483648
  %"^_tmp40" = xor i32 %66, %load_var_R_EBX_3230
  %"&_tmp41" = and i32 %"^_tmp40", %"^_tmp37"
  %cast_high43 = icmp slt i32 %"&_tmp41", 0
  store i1 %cast_high43, i1* @R_OF, align 1
  %load_var_R_EBX_3244 = load i32* @R_EBX_32, align 4
  %"^_tmp46" = xor i32 %load_var_R_EBX_3244, %load_var_R_EBX_3230
  %"&_tmp49" = and i32 %"^_tmp46", 16
  %"==_tmp50" = icmp ne i32 %"&_tmp49", 0
  store i1 %"==_tmp50", i1* @R_AF, align 1
  %">>_tmp52" = lshr i32 %load_var_R_EBX_3244, 4
  %"^_tmp54" = xor i32 %">>_tmp52", %load_var_R_EBX_3244
  %">>_tmp55" = lshr i32 %"^_tmp54", 2
  %"^_tmp56" = xor i32 %">>_tmp55", %"^_tmp54"
  %">>_tmp57" = lshr i32 %"^_tmp56", 1
  %"^_tmp58" = xor i32 %">>_tmp57", %"^_tmp56"
  %67 = and i32 %"^_tmp58", 1
  %bwnot60 = icmp eq i32 %67, 0
  store i1 %bwnot60, i1* @R_PF, align 1
  %load_var_R_EBX_3261 = load i32* @R_EBX_32, align 4
  %cast_high63 = icmp slt i32 %load_var_R_EBX_3261, 0
  store i1 %cast_high63, i1* @R_SF, align 1
  %"==_tmp65" = icmp eq i32 %load_var_R_EBX_3261, 0
  store i1 %"==_tmp65", i1* @R_ZF, align 1
  br label %BB_4

BB_2:                                             ; preds = %BB_1, %switch
  %load_var_R_ESP_3226 = load i32* @R_ESP_32, align 4
  %-_tmp27 = add i32 %load_var_R_ESP_3226, -4
  store i32 %-_tmp27, i32* @R_ESP_32, align 4
  %load_address29 = inttoptr i32 %-_tmp27 to i32*
  store i32 134513501, i32* %load_address29, align 4
  br label %BB_44

BB_1:                                             ; preds = %BB_0, %switch
  %load_var_R_ESP_322 = load i32* @R_ESP_32, align 4
  %-_tmp4 = add i32 %load_var_R_ESP_322, -8
  store i32 %-_tmp4, i32* @R_ESP_32, align 4
  %"<_tmp" = icmp ult i32 %load_var_R_ESP_322, 8
  store i1 %"<_tmp", i1* @R_CF, align 1
  %68 = sub i32 7, %load_var_R_ESP_322
  %"&_tmp" = and i32 %68, %load_var_R_ESP_322
  %cast_high = icmp slt i32 %"&_tmp", 0
  store i1 %cast_high, i1* @R_OF, align 1
  %load_var_R_ESP_329 = load i32* @R_ESP_32, align 4
  %"^_tmp11" = xor i32 %load_var_R_ESP_329, %load_var_R_ESP_322
  %"&_tmp13" = and i32 %"^_tmp11", 16
  %"==_tmp" = icmp ne i32 %"&_tmp13", 0
  store i1 %"==_tmp", i1* @R_AF, align 1
  %">>_tmp" = lshr i32 %load_var_R_ESP_329, 4
  %"^_tmp16" = xor i32 %">>_tmp", %load_var_R_ESP_329
  %">>_tmp17" = lshr i32 %"^_tmp16", 2
  %"^_tmp18" = xor i32 %">>_tmp17", %"^_tmp16"
  %">>_tmp19" = lshr i32 %"^_tmp18", 1
  %"^_tmp20" = xor i32 %">>_tmp19", %"^_tmp18"
  %69 = and i32 %"^_tmp20", 1
  %bwnot = icmp eq i32 %69, 0
  store i1 %bwnot, i1* @R_PF, align 1
  %load_var_R_ESP_3221 = load i32* @R_ESP_32, align 4
  %cast_high23 = icmp slt i32 %load_var_R_ESP_3221, 0
  store i1 %cast_high23, i1* @R_SF, align 1
  %"==_tmp25" = icmp eq i32 %load_var_R_ESP_3221, 0
  store i1 %"==_tmp25", i1* @R_ZF, align 1
  br label %BB_2
}

declare void @"*804a00c__gmon_start__"(i32)

declare void @"*804a010__gmon_start__"(i32)

declare void @"*804a014__libc_start_main@GLIBC_2.0"(i32)

declare void @"*804a018fopen@GLIBC_2.1"(i32)

declare void @"*804a01c__isoc99_scanf@GLIBC_2.7"(i32)

declare { i32, i1 } @llvm.uadd.with.overflow.i32(i32, i32) nounwind readnone

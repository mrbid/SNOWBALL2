#ifndef ICON_H
#define ICON_H

// Graphical Attribution
// mouse_image: https://www.flaticon.com/free-icon/computer-mouse_1385
// bg_image[sappling]: https://www.shutterstock.com/image-illustration/tree-growth-diagram-green-leaf-nature-1299130129
// icon_image & icon_image1: http://www.forrestwalter.com/icons/

static const struct {
  unsigned int  width;
  unsigned int  height;
  unsigned int  bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char pixel_data[16 * 16 * 4 + 1];
} icon_image = {
  16, 16, 4,
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\377\000\000\000\377\000\000\000\377\000"
  "\000\000\377\000\000\000\377\000\000\000\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\021\377\000\000\000\377\000\000\314\377\000\063\314\377\000\000\314"
  "\377\000\063\377\377\000\000\314\377\000\000\335\377\000\000\000\377\000\000\021\377\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\"\377\000\000\273\377\000\063\314\377\000f\377"
  "\377\063\231\377\377\000\231\377\377\063\231\377\377\063f\377\377\000f\314\377\000"
  "\000\335\377\000\000\314\377\000\000\"\377\000\000\000\000\000\000\000\000\000\000\000\000\021\000\000\377\000\000\252"
  "\377\000\063\377\377\063\063\377\377\063\231\314\377\063\314\377\377f\314\377\377"
  "f\231\377\377\063\231\377\377\000\063\377\377\000f\314\377\000\063\377\377\000\000\335"
  "\377\000\000\021\377\000\000\000\000\000\000\000\000\000\000\252\377\000\000\314\377\000\063\314\377\063\231"
  "\377\377f\314\377\377\231\377\377\377f\314\377\377\063\231\377\377f\314\377"
  "\377\000f\377\377\000\063\314\377\000\063\231\377\000\063\314\377\000\000\000\377\000\000\000\000"
  "\000\000\000\377\000\000\273\377\000\063\231\377\000\063\377\377\000\231\377\377f\231\377\377"
  "f\314\377\377f\314\377\377\063\231\314\377\000f\377\377\063f\314\377\000f\377\377"
  "\000\063\231\377\000\000f\377\000\000\252\377\000\000\000\377\000\000\000\377\000\000\252\377\000f\314"
  "\377\000f\377\377\000\063\314\377\000f\377\377\063\231\377\377\063f\377\377\063\231"
  "\377\377\063\231\314\377\000\063\377\377\000\063\231\377\000\063\314\377\000\063f\377"
  "\000\000\231\377\000\000\000\377\000\000\000\377\000\000w\377\000\063\314\377\000\063\314\377\000f\377"
  "\377\000f\377\377\000f\377\377\000\231\314\377\000f\377\377\000f\377\377\000\063\377\377"
  "\000\063\314\377\000\000U\377\000\063\231\377\000\000f\377\000\000\000\377\000\000\000\377\000\063\231"
  "\377\000\063\231\377\000\000\335\377\000\000\314\377\000\063\377\377\000f\314\377\000\063\377"
  "\377\000\063\314\377\000\063\377\377\000\000\314\377\000\063\231\377\000\000f\377\000\000U\377"
  "\000\000\210\377\000\000\000\377\000\000\000\377\000\000\252\377\000\000w\377\000\000\231\377\000\000\252"
  "\377\000\063\231\377\000\063\377\377\000\000\335\377\000\063\377\377\000\000\314\377\000\063"
  "\231\377\000\000f\377\000\000w\377\000\063f\377\000\000w\377\000\000\000\377\000\000\000\377\000\000\252"
  "\377\000\063\231\377\000\000w\377\000\063\314\377\000\000w\377\000\000\231\377\000\000\231\377"
  "\000\063\314\377\000\000f\377\000\000\252\377\000\000\210\377\000\063f\377\000\063\231\377\000\000"
  "\231\377\000\000\000\377\000\000\000\000\000\000\000\377\000\000w\377\000\063f\377\000\000U\377\000\000f\377"
  "\000\000f\377\000\000f\377\000\000f\377\000\000w\377\000\000U\377\000\000D\377\000\063f\377\000\000D\377"
  "\000\000\000\377\000\000\000\000\000\000\000\000\000\000\000\377\000\000\231\377\000\063f\377\000\000w\377\000\000f"
  "\377\000\000D\377\000\000w\377\000\000w\377\000\063f\377\000\063\231\377\000\063\231\377\000\000w"
  "\377\000\000\273\377\000\000\021\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000\"\377\000\000\210\377"
  "\000\000f\377\000\063f\377\000f\231\377\000\063\231\377\000\063f\377\000\063\231\377\000\063\231"
  "\377\000\000f\377\000\000\252\377\000\000\"\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\021\377\000\000\000\377\000\000U\377\000\000f\377\000\000f\377\000\000D\377\000\000w\377\000\000"
  "\210\377\000\000\000\377\000\000\021\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000\000\000\377\000"
  "\000\000\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
};

static const struct {
  unsigned int  width;
  unsigned int  height;
  unsigned int  bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char pixel_data[16 * 16 * 4 + 1];
} icon_image1 = {
  16, 16, 4,
  "\000\000\000\000\000\000\000\000\000\000\000\000\023\023\023\005!!!E\035\035\035\205$\040\026\372=\033\033\377"
  "M\032\030\377&\023\017\277\016\002\001{\000\001\006\017\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\"\"\"\012\037\037\037\265++-\372,,,\377A/.\377\\((\377\244\061"
  "#\377\252+\026\377n\037\027\377\067\030\006\360\003\001\001I\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\022\022\022\012\037\037#\266//B\377\067\070\\\377WVZ\377J?B\377;\064\065\377"
  "\207-,\377\354\065#\377\344Q\031\377\231\067\025\377B\032\004\365\003\001\000I\000\000\000\000"
  "\000\000\000\000\002\002\002\005\026\026\027\265++Q\377C\066v\377[c\215\377\216\216\263\377l"
  "q\213\377QGL\377H\065\065\377\233D+\377\374v\"\377\361\222#\377\270T\002\377"
  "@\037\000\365\000\000\000E\000\000\000\000\004\004\004E!\040>\372\062,\222\377\071\070\234\377xx\266"
  "\377\317\317\353\377\267\256\322\377ei\201\377;\070H\377e\062%\377\345\201"
  "\"\377\377\275!\377\361\237\026\377\222Q\000\377\017\004\000\272\000\000\000\005\031\031\036"
  "\205\037\037q\377\064\062\277\377OD\300\377ok\312\377\261\261\332\377|\212\270"
  "\377RS\203\377\063\064[\377F(\060\377\276V\032\377\375\217\026\377\376\260\040"
  "\377\343\216\002\377M:\000\365\000\000\000\017\013\014\031\372#\"\233\377,,\315\377\065"
  "\063\314\377PG\314\377^^\302\377Q[\263\377\066\067\205\377-%g\377*'G\377u(\030"
  "\377\320Q\025\377\363`\004\377\373\243\000\377\236f\000\377\023\006\000{\007\007=\377\030"
  "\031\262\377\"#\341\377,+\345\377\064\062\332\377\065\064\313\377\064\065\267\377"
  "\063A\245\377+$w\377)\"h\377P\011\061\377\217!\006\377\323\035\024\377\357z\000\377"
  "\303\220\000\377-\033\000\177\007\024\067\377\026\025\252\377\025\030\342\377\"\031\351"
  "\377$*\327\377$$\324\377+$\312\377$&\256\377\"#\224\377\032\"v\377-\026\\\377"
  "\210\004\030\377\274\025\002\377\327B\000\377\274x\000\377%\024\000\177\004\006\025\372\024"
  "\003\216\377\004\026\333\377!\025\350\377\031\031\341\377\040!\325\377\033\"\311\377"
  "%#\305\377;*\250\377\031\030\213\377\034\025\213\377{\002/\377\253\000\000\377\272"
  "\010\000\377\206)\000\377\002\001\000{\001\010\010\200\004\000b\377\024\024\317\377\006\004\365\377"
  "\025\025\344\377\026\004\322\377\037\025\304\377\030\030\275\377%\032\260\377\026\025"
  "\235\377\016\002\225\377v\000b\377\236\000\002\377\226\000\000\377D\002\000\360\000\000\000\017\001"
  "\006\000\017\000\004\062\360\002\000\234\377\000\024\360\377\000\002\373\377\002\024\346\377\002\004"
  "\300\377\024\002\261\377\026\024\256\377\030\030\252\377\027\040\237\377r\002u\377\241"
  "\000\005\377v\000\000\377\015\000\000\177\000\000\000\000\000\000\000\000\000\001\011\177\000\004Z\377\000\000\272"
  "\377\000\000\361\377\000\002\367\377\000\000\302\377\002\000\235\377\000\000\226\377\032\026\237"
  "\377\060\026\260\377a\000\202\377\216\000'\377\066\004\002\360\000\006\000\017\000\000\000\000\000\000"
  "\000\000\000\001\000\012\000\000\014\266\004\000[\377\000\000\257\377\000\000\316\377\000\000\247\377\000\000"
  "n\377\000\000e\377\001\000\200\377\012\004\216\377<\001p\377&\004\035\365\003\001\001I\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\001\000\000\012\000\000\012\265\004\004:\372\001\000h\377\000\000g\377\000"
  "\000C\377\000\000\066\377\000\000@\377\011\027\070\372\016\016\027\272\001\001\001E\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\002\000\005\001\003\001E\010\010\014\205\004\004\010\372"
  "\000\004\022\377\000\004\017\377\000\005\006\277\003\003\002E\021\002\002\005\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000"
};

static const struct {
  unsigned int  width;
  unsigned int  height;
  unsigned int  bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char pixel_data[16 * 16 * 4 + 1];
} icon_image2 = {
  16, 16, 4,
  "\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000\000\000\000\000\003\000\000\000\065\021\020"
  "\016dnd<\261}sG\261%!\021~\000\000\000\070\000\000\000\003\377\377\377\000\377\377\377\000\325"
  "\326\326\000\212\206\177\000\377\377\377\000\377\377\377\000\032\031\026\000H?\034O}sG"
  "\333\231\231f\374\270\261\205\376\315\304\223\377\326\320\246\377\315\304"
  "\223\377\254\243p\374ul=\274\000\000\000\033\000\000\000\000\377\377\377\000\257\264\267\000"
  "\377\377\377\000\021\020\016\001\067-\022V\254\243p\362\335\324\226\377\337\324\207"
  "\377\314\301t\377\305\270[\377\266\247`\377\266\247`\377\302\264p\377\270"
  "\261t\377\223\212R\345%!\021V\021\020\016\001\377\377\377\000\021\020\016\002%!\021^\231"
  "\231f\365\337\324\207\377\314\301t\377\304\273N\377\274\257;\377\254\236"
  "!\377\250\240\063\377\235\225:\377\235\225:\377\235\225:\377\270\261t\377"
  "\231\231f\365DB/z\000\000\000\013\000\000\000\071\223\212R\365\315\304\223\377\304\273"
  "N\377\304\273N\377\274\257;\377\235\224\032\377\235\224\032\377\235\224\032"
  "\377\223\215\026\377\213\205\032\377\211\201%\377\222\214%\377\270\261t\377"
  "\223\212R\374\000\000\000A\067-\022\204\302\264p\377\324\307v\377\270\253\060\377"
  "\270\253\060\377\254\236!\377\215\205\015\377\201{\021\377\201{\021\377|r\011"
  "\377|r\011\377\201{\021\377{q\021\377\202m&\377\216\203D\377\034\020\015\204[L"
  "\035\206\314\301t\377\332\310`\377\262\237,\377\254\236!\377\201{\021\377s"
  "m\010\377\203z\011\377|r\011\377sm\010\377|r\011\377\201{\021\377{q\021\377uk\033"
  "\377}sG\377H?\034\260vb#\312\305\270[\377\276\265B\377\254\236!\377\213\205"
  "\032\377bR\002\377bR\002\377\215\205\015\377\203z\011\377\215\205\015\377\203z\011"
  "\377\215\205\015\377{q\021\377uk\033\377vb#\377H?\034\345vb#\341\254\245B\377"
  "\262\237,\377\213\205\032\377bR\002\377[K\001\377[K\001\377j_\020\377\203z\011\377"
  "\203z\011\377\203z\011\377\203z\011\377\203z\011\377j_\020\377l[\033\377[L\035\345"
  "J:\013\222\225~'\377\215v\"\377w`\014\377kR\004\377kR\004\377cK\003\377cK\003\377k"
  "R\004\377kR\004\377kR\004\377kR\004\377kR\004\377hS\020\377l[\033\377H?\034\342\000\000\000"
  "\\\202m&\375\207o\031\377w`\014\377s\\\016\377kR\004\377kR\004\377cK\003\377[K\001\377"
  "bR\002\377bR\002\377cK\003\377cL\011\377hS\020\377vb#\377\067-\022\224\000\000\000*eQ\040"
  "\365uk\033\377w`\014\377w`\014\377kR\004\377cK\003\377SB\004\377SB\004\377SB\004\377["
  "K\001\377cL\011\377cL\011\377l[\033\377eQ\040\367\021\020\016V\021\020\016\002\067-\022^"
  "eQ\040\365hS\020\377hS\020\377hS\020\377[K\001\377SB\004\377SB\004\377SB\004\377SB\004"
  "\377[J\020\377[J\020\377[L\035\365%!\021^\021\020\016\001\377\377\377\000\021\020\016\001"
  "#\027\021VTB\032\362[J\020\377TC\017\377J:\013\377J:\013\377J:\013\377J:\013\377J"
  ":\013\377TC\017\377TB\032\362#\027\021V\021\020\016\001\377\377\377\000\377\377\377\000"
  "\377\377\377\000\032\031\026\000\000\000\000EJ:\013\312J:\013\376J:\013\377J:\013\377J:\013"
  "\377J:\013\376H?\034\374\067-\022\333%!\021O\032\031\026\000\377\377\377\000\377\377"
  "\377\000\377\377\377\000\377\377\377\000\377\377\377\000\377\377\377\000\000\000\000\023\000"
  "\000\000N\034\020\015\211\067-\022\224\067-\022\254\021\020\016d\000\000\000\065\000\000\000\003\000\000\000"
  "\000\377\377\377\000\377\377\377\000\377\377\377\000"
};

#endif

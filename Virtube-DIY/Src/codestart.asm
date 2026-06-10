; assembler functions

; startup code

        ; external reference _c_int00
        .ref _c_int00
        .def _Entry
        ; start

        .sect "codestart"
_Entry:
        ; start c software
        lb _c_int00

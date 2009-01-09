AC_DEFUN([EXPAND],
[
  EXP_VAR=[$1]
  FROM_VAR=[$2]
dnl first expand prefix and exec_prefix if necessary
  prefix_save=$prefix
  exec_prefix_save=$exec_prefix
# normally, this is done automatically first when running AC_OUTPUT, but we need them expanded now
  test "x$prefix" = xNONE && prefix=$ac_default_prefix
# Let make expand exec_prefix.
  test "x$exec_prefix" = xNONE && exec_prefix='${prefix}'
  full_var="$FROM_VAR"
dnl loop until it doesn"t change anymore
  while true; do
    new_full_var="`eval echo $full_var`"
    if test "x$new_full_var" = "x$full_var"; then break; fi
    full_var=$new_full_var
  done
dnl clean up
  full_var=$new_full_var
  AC_SUBST([$1], "$full_var")
dnl restore prefix and exec_prefix
  prefix=$prefix_save
  exec_prefix=$exec_prefix_save
])

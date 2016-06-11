#! /bin/csh -f

set modname=genmod
set min_node=12
set max_node=12
set step=4
set train_epoch=120
set batchsize=10
set momentum=0.8
set use_costs=false

../../template/modfile.pl $modname $min_node $max_node $step $train_epoch ../../template/basename.mod  \
> $modname.mod
ln -s ../scale/$modname.scl $modname.scl
ln -s ../scale/$modname.tag $modname.tag
ln -s ../scale/$modname.cfg $modname.cfg

foreach num_hidden_node (10)
  cfg2nn $modname -m=$momentum -n=$num_hidden_node -s=$batchsize -r=139317 -i=$train_epoch
  nnet -c$modname.nncfg -o ${modname}_nn_$num_hidden_node.nnw -v -t backprop $modname.scl
  mv $modname.nncfg ${modname}_nn_$num_hidden_node.nncfg
  nn2wta ${modname}_nn_$num_hidden_node.nnw ${modname}_nn_$num_hidden_node
end



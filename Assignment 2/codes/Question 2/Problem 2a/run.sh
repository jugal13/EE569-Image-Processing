g++ Prog2a.cpp && \
echo "Method $1" && \
if [[ $1 ==  'matrix' ]]; then
  echo "Matrix Size: $2"
else
  echo
fi && \
./a.out ../../../images/Lighthouse.raw Output.raw $1 $2

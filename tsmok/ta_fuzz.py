import tsmok.fuzzing.sys_fuzzer as fuzz
from collections import defaultdict

class TaFuzzingOps:
  """Operation Context for TA fuzzing."""

  def __init__(self, ta):
    self._ta = ta
    self._allocated_mem_regs = []
    self._nr_arg_handler = defaultdict(lambda: defaultdict(lambda: None))

  def get_ctx(self):
    return fuzz.OpsCtx(self._ta.forkserver_start,
                       self._ta.syscall,
                       lambda: self._ta.exit(0),
                       self._arg_value_checker,
                       self._load_args_to_mem,
                       self._ta.mem_read,
                       self._cleanup)
    
  def _arg_value_checker(self, num, name, value):
    if num in self._nr_arg_handler and name in self._nr_arg_handler[num]:
        return self._nr_arg_handler[num][name](value)
    return value

  def _load_args_to_mem(self, addr, data):
    to_addr = addr
    if not to_addr:
      reg = self._ta.allocate_shm_region(len(data))
      to_addr = reg
      self._allocated_mem_regs.append(reg)
    self._ta.mem_write(to_addr, data)
    return to_addr

  def _cleanup(self):
    for r in self._allocated_mem_regs:
      self._ta.free_shm_region(r)
      self._allocated_mem_regs.remove(r)

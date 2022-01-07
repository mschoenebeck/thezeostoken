module.exports = async ({ proto, address }) => {
  // zeos_echo://

  return new Buffer(address, "utf-8");
};

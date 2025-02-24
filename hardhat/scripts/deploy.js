// scripts/deploy.js
async function main () {
  // Deploying EternalStorage
  const EternalStorage = await ethers.getContractFactory('EternalStorage');
  console.log('Deploying EternalStorage...');
  const eternalStorage = await EternalStorage.deploy();
  await eternalStorage.waitForDeployment();
  console.log('EternalStorage deployed to:', await eternalStorage.getAddress());
  
  // Deploying AlarmStorage
  const AlarmStorage = await ethers.getContractFactory('AlarmStorage');
  console.log('Deploying AlarmStorage...');
  const alarmStorage = await AlarmStorage.deploy();
  await alarmStorage.waitForDeployment();
  console.log('AlarmStorage deployed to:', await alarmStorage.getAddress());
}

main()
  .then(() => process.exit(0))
  .catch(error => {
    console.error(error);
    process.exit(1);
  });

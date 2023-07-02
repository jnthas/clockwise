# Release Checklist

Just a checklist to follow the steps to be done when releasing a new version of Clockwise. I know, I must automate this. 

- [ ] Update website in [gh-pages](https://github.com/jnthas/clockwise/blob/gh-pages/index.md)
  - [ ] Update message at the top with most significant feature released 
- [ ] Update the [CHANGELOG.md](https://github.com/jnthas/clockwise/blob/main/CHANGELOG.md)
- [ ] Update [README.md](https://github.com/jnthas/clockwise/blob/main/README.md)
  - [ ] Update message at the top with most significant feature released 
  - [ ] Update Params section if new ones were added
- [ ] Update firmware version in [platformio.ini](https://github.com/jnthas/clockwise/blob/aa22923dc195a3ffff8e95766e7ec9acda82e090/firmware/platformio.ini#L39)
- [ ] For new clockfaces
  - [ ] Add new folder with the clockface code in the branch [gh-pages](https://github.com/jnthas/clockwise/tree/gh-pages/static/firmware/cw-cf-0x01)
  - [ ] Include the new clockface in the [CI/CD file](https://github.com/jnthas/clockwise/blob/main/.github/workflows/clockwise-ci.yml)
  - [ ] Add the [CMakeLists.txt](https://github.com/jnthas/cw-cf-0x07/blob/12eb5e70f0d4993d8531b871ad02f3964b76e582/CMakeLists.txt) in the new clokface repository 
- [ ] Check if [ESP-IDF CI](https://github.com/jnthas/clockwise/actions) is passing without errors 
- [ ] Create a branch called releases/1.x.x (it will trigger the build and release)
- [ ] Create a tag (add the same description of the CHANGELOG)

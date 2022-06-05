import cv2

img = cv2.imread("../../image/cat.jpeg")

detector = cv2.CascadeClassifier(cv2.data.haarcascades + "haarcascade_frontalcatface.xml")

faces = detector.detectMultiScale(img)
print(faces)
for (x, y, w, h) in faces:
    cv2.rectangle(img, (x, y), (x + w, y + h), (255, 0, 0), 3)

cv2.imshow("cat!!!", img)
cv2.waitKey(0)

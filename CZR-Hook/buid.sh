clear
start=$(date +%s)
cd jni
/data/x0/ndk/ndk-build
mv ../libs/arm64-v8a/libhook.so /data/user/0/com.pi.czrxdfirst/
end=$(date +%s)
diff=$(( end - start ))
echo "编译总耗时: $diff s"

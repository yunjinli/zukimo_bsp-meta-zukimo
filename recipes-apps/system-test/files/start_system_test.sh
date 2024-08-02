#! /bin/bash


echo "#########################################################################"
echo "        Starting System Test"
echo "#########################################################################"


# Cleaning up any previous generated test reports
rm -f /tmp/test_system*

/usr/bin/system_test --gtest_output=xml:/tmp/

# As we now use google test directly as submodule we can not rely on our patched version anymore.
# So we just add the line that specifies our xsl template with the help of sed
sed -i '2 i <?xml-stylesheet type="text/xsl" href="gtest-result.xsl" ?>' /tmp/system_test.xml

while true; do
    echo "Test finished, remove test binaries? (y/n)"
    read answer
    if [ "$answer" == "y" ]; then
        mount / -o remount,rw

        # Automatically remove all test binaries and test script from the ecu
        rm -f /usr/bin/start_system_test.sh   || true
        rm -f /usr/bin/system_test  || true

        break
    elif [ "$answer" == "n" ]; then
        break
    fi
done

echo "#########################################################################"
echo "#    System Test finished                                               #"
echo "#########################################################################"

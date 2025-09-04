# So sánh kiến trúc CISC và RISC

### 1. Giới thiệu khái niệm
- **CISC (Complex Instruction Set Computer)**: Kiến trúc máy tính với tập lệnh phức tạp, mỗi lệnh có thể thực hiện nhiều thao tác cấp thấp trong một lần gọi.
- **RISC (Reduced Instruction Set Computer)**: Kiến trúc máy tính với tập lệnh rút gọn, đơn giản hóa và tối ưu hóa tốc độ xử lý.

### 2. Ưu điểm và nhược điểm

**CISC**
- Ưu điểm:  
  - Lập trình dễ dàng hơn do ít dòng lệnh hơn.  
  - Tối ưu hóa cho ngôn ngữ bậc cao.  
- Nhược điểm:  
  - Mạch xử lý phức tạp.  
  - Tốc độ xử lý lệnh chậm hơn.  

**RISC**
- Ưu điểm:  
  - Tốc độ xử lý nhanh nhờ tập lệnh đơn giản.  
  - Thiết kế phần cứng gọn nhẹ, tiết kiệm năng lượng.  
- Nhược điểm:  
  - Cần nhiều dòng lệnh hơn cho một tác vụ.  
  - Lập trình phức tạp hơn đối với một số chức năng.  

### 3. So sánh CISC và RISC theo các tiêu chí

| Tiêu chí                     | CISC                                | RISC                                 |
|-------------------------------|--------------------------------------|---------------------------------------|
| 1. Cấu trúc tập lệnh          | Nhiều lệnh phức tạp                  | Ít lệnh, đơn giản, thống nhất         |
| 2. Tốc độ xử lý               | Chậm hơn do giải mã phức tạp         | Nhanh hơn nhờ pipeline và lệnh ngắn   |
| 3. Kích thước chương trình    | Nhỏ hơn                              | Lớn hơn (cần nhiều lệnh hơn)          |
| 4. Độ phức tạp phần cứng      | Rất phức tạp                         | Đơn giản, dễ mở rộng                  |
| 5. Ứng dụng thực tế           | Intel x86, AMD                      | ARM, MIPS, RISC-V                     |

### 4. Quan điểm cá nhân
Trong bối cảnh phát triển hệ thống nhúng hiện nay, kiến trúc **RISC** (đặc biệt là ARM) phù hợp hơn vì:  
- Tốc độ xử lý cao, hỗ trợ pipeline hiệu quả.  
- Tiết kiệm năng lượng, rất quan trọng trong thiết bị di động và IoT.  
- Dễ dàng mở rộng và tích hợp vào nhiều hệ thống khác nhau.  

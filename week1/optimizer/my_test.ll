; ModuleID = 'test.ll'
source_filename = "test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @func(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  store i32 %0, ptr %3, align 4
  store i32 10, ptr %6, align 4
  %8 = load i32, ptr %3, align 4
  %9 = add nsw i32 %8, 10
  store i32 %9, ptr %4, align 4
  store i32 %9, ptr %5, align 4
  store i32 %9, ptr %7, align 4
  store i32 10, ptr %4, align 4
  br label %10

10:                                               ; preds = %13, %1
  %11 = load i32, ptr %3, align 4
  %12 = icmp slt i32 %11, 100
  br i1 %12, label %13, label %16

13:                                               ; preds = %10
  %14 = load i32, ptr %3, align 4
  %15 = add nsw i32 %14, 5
  store i32 %15, ptr %3, align 4
  store i32 %15, ptr %6, align 4
  store i32 %15, ptr %7, align 4
  br label %10, !llvm.loop !6

16:                                               ; preds = %10
  %17 = load i32, ptr %7, align 4
  %18 = icmp sgt i32 %17, 100
  br i1 %18, label %19, label %21

19:                                               ; preds = %16
  %20 = load i32, ptr %6, align 4
  store i32 %20, ptr %2, align 4
  br label %22

21:                                               ; preds = %16
  store i32 100, ptr %2, align 4
  br label %22

22:                                               ; preds = %21, %19
  %23 = load i32, ptr %2, align 4
  ret i32 %23
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Ubuntu clang version 15.0.7"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
